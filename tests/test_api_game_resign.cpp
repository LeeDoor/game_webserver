#include "socket_functions.hpp"
#include "api_functions.hpp"

#include "session.hpp"

#include "json_type_converter.hpp"
#include "type_serializer.hpp"


TEST_CASE("ApiResign", "[api][game][move][resign]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    
    std::string ACCESS_DENIED = serializer::SerializeError("access_denied", "you have no access to do this action");
    std::string SESSION_FINISHED = serializer::SerializeError("session_finished", "session is finished");

    SECTION("success"){
        SessionData sd = CreateNewMatch(socket);
        um::Login& now_turn = sd.state.now_turn;
        LoginData& ld = sd.l1.login == now_turn ? sd.l1 : sd.l2;
        ResignSuccess(socket, ld.token, sd.sid);
        sm::PublicSessionData psd = PublicSessionDataSuccess(socket, sd.sid, sd.l1.token);
        CHECK(psd == PublicSessionDataSuccess(socket, sd.sid, sd.l2.token));
        CHECK(*psd.winner != *psd.loser);
        CHECK(*psd.loser == ld.login);


        INFO("event list contains resign and match end");
        {
            StringResponse events_resp = SessionStateChange(socket, sd.l1.token, sd.sid, 1);
            nlohmann::json j = nlohmann::json::parse(events_resp.body());
            INFO(j.dump());
            REQUIRE(j.is_array());
            REQUIRE(j.size() == 2);
            CHECK(j[0]["event_type"] == "player_resign");
            CHECK(j[0]["actor_id"] == 0);
            CHECK(j[0]["move_number"] == 1);

            CHECK(j[1]["event_type"] == "player_won");
            CHECK(j[1]["actor_id"] == 1);
            CHECK(j[1]["move_number"] == 1);
        }
    }
    SECTION("success_when_not_our_move"){
        SessionData sd = CreateNewMatch(socket);
        um::Login& now_turn = sd.state.now_turn;
        LoginData& ld1 = sd.l1.login == now_turn ? sd.l1 : sd.l2;
        LoginData& ld2 = sd.l1.login == now_turn ? sd.l2 : sd.l1;
        ResignSuccess(socket, ld2.token, sd.sid);
        INFO("resign is successful");
        sm::PublicSessionData psd = PublicSessionDataSuccess(socket, sd.sid, sd.l1.token);
        CHECK(psd == PublicSessionDataSuccess(socket, sd.sid, sd.l2.token));
        CHECK(*psd.winner != *psd.loser);
        CHECK(*psd.winner == ld1.login);
        CHECK(*psd.loser == ld2.login);
    }
    SECTION("access_denied"){
        SessionData sd = CreateNewMatch(socket);
        LoginData ld = EnqueueNewPlayer(socket);
        StringResponse response = Resign(socket, ld.token, sd.sid);
        CheckStringResponse(response, {
            .body = ACCESS_DENIED,
            .res = http::status::bad_request
        });
        gm::State state = SessionStateSuccess(socket, sd.l1.token, sd.sid);
        CHECK(state == SessionStateSuccess(socket, sd.l2.token, sd.sid));
    }
    SECTION("cant_resign_twice"){
        SessionData sd = CreateNewMatch(socket);
        um::Login& now_turn = sd.state.now_turn;
        LoginData& ld = sd.l1.login == now_turn ? sd.l1 : sd.l2;
        ResignSuccess(socket, ld.token, sd.sid);
        INFO("resign is successful");
        sm::PublicSessionData psd = PublicSessionDataSuccess(socket, sd.sid, sd.l1.token);
        CHECK(psd == PublicSessionDataSuccess(socket, sd.sid, sd.l2.token));
        CHECK(*psd.winner != *psd.loser);
        CHECK(*psd.loser == ld.login);
        
        StringResponse response = Resign(socket, sd.l1.token, sd.sid);
        CheckStringResponse(response, {
            .body=SESSION_FINISHED,
            .res=http::status::bad_request
        });

        response = Resign(socket, sd.l2.token, sd.sid);
        CheckStringResponse(response, {
            .body=SESSION_FINISHED,
            .res=http::status::bad_request
        });
    }
    SECTION("cant_move_after_resign"){
        SessionData sd = CreateNewMatch(socket);
        um::Login& now_turn = sd.state.now_turn;
        LoginData& ld = sd.l1.login == now_turn ? sd.l1 : sd.l2;
        ResignSuccess(socket, ld.token, sd.sid);
        INFO("resign is successful");
        sm::PublicSessionData psd = PublicSessionDataSuccess(socket, sd.sid, sd.l1.token);
        CHECK(psd == PublicSessionDataSuccess(socket, sd.sid, sd.l2.token));
        CHECK(*psd.winner != *psd.loser);
        CHECK(*psd.loser == ld.login);
        
        gm::Player& player1 = *(sd.state.players.front()->login == sd.l1.login ? sd.state.players.front(): sd.state.players.back());
        gm::Player& player2 = *(sd.state.players.back()->login == sd.l1.login ? sd.state.players.front(): sd.state.players.back()); 

        StringResponse response = Walk(socket, {player1.posX + 1, player1.posY}, sd.l1.token, sd.sid);
        CheckStringResponse(response, {
            .body=SESSION_FINISHED,
            .res=http::status::bad_request
        });

        response = Walk(socket, {player2.posX + 1, player2.posY}, sd.l1.token, sd.sid);
        CheckStringResponse(response, {
            .body=SESSION_FINISHED,
            .res=http::status::bad_request
        });
    }
}