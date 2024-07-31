#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "session.hpp"
using namespace serializer;

TEST_CASE("ApiResign", "[api][game][resign]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string ACCESS_DENIED = serializer->SerializeError("access_denied", "you have no access to do this action");
    std::string SESSION_FINISHED = serializer->SerializeError("session_finished", "session is finished");

    SECTION("success"){
        SessionData sd = CreateNewMatch(socket, serializer);
        um::Login& now_turn = sd.state.now_turn;
        LoginData& ld = sd.l1.login == now_turn ? sd.l1 : sd.l2;
        ResignSuccess(socket, ld.token, sd.sid);
        INFO("resign is successful");
        sm::PublicSessionData psd = PublicSessionDataSuccess(socket, serializer, sd.sid, sd.l1.token);
        CHECK(psd == PublicSessionDataSuccess(socket, serializer, sd.sid, sd.l2.token));
        CHECK(*psd.winner != *psd.loser);
        CHECK(*psd.loser == ld.login);
    }
    SECTION("success_when_not_our_move"){
        SessionData sd = CreateNewMatch(socket, serializer);
        um::Login& now_turn = sd.state.now_turn;
        LoginData& ld1 = sd.l1.login == now_turn ? sd.l1 : sd.l2;
        LoginData& ld2 = sd.l1.login == now_turn ? sd.l2 : sd.l1;
        ResignSuccess(socket, ld2.token, sd.sid);
        INFO("resign is successful");
        sm::PublicSessionData psd = PublicSessionDataSuccess(socket, serializer, sd.sid, sd.l1.token);
        CHECK(psd == PublicSessionDataSuccess(socket, serializer, sd.sid, sd.l2.token));
        CHECK(*psd.winner != *psd.loser);
        CHECK(*psd.winner == ld1.login);
        CHECK(*psd.loser == ld2.login);
    }
    SECTION("access_denied"){
        SessionData sd = CreateNewMatch(socket, serializer);
        LoginData ld = EnqueueNewPlayer(socket, serializer);
        StringResponse response = Resign(socket, ld.token, sd.sid);
        CheckStringResponse(response, {
            .body = ACCESS_DENIED,
            .res = http::status::bad_request
        });
        gm::State state = SessionStateSuccess(socket, serializer, sd.l1.token, sd.sid);
        CHECK(state == SessionStateSuccess(socket, serializer, sd.l2.token, sd.sid));
    }
    SECTION("cant_resign_twice"){
        SessionData sd = CreateNewMatch(socket, serializer);
        um::Login& now_turn = sd.state.now_turn;
        LoginData& ld = sd.l1.login == now_turn ? sd.l1 : sd.l2;
        ResignSuccess(socket, ld.token, sd.sid);
        INFO("resign is successful");
        sm::PublicSessionData psd = PublicSessionDataSuccess(socket, serializer, sd.sid, sd.l1.token);
        CHECK(psd == PublicSessionDataSuccess(socket, serializer, sd.sid, sd.l2.token));
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
        SessionData sd = CreateNewMatch(socket, serializer);
        um::Login& now_turn = sd.state.now_turn;
        LoginData& ld = sd.l1.login == now_turn ? sd.l1 : sd.l2;
        ResignSuccess(socket, ld.token, sd.sid);
        INFO("resign is successful");
        sm::PublicSessionData psd = PublicSessionDataSuccess(socket, serializer, sd.sid, sd.l1.token);
        CHECK(psd == PublicSessionDataSuccess(socket, serializer, sd.sid, sd.l2.token));
        CHECK(*psd.winner != *psd.loser);
        CHECK(*psd.loser == ld.login);
        
        gm::Player& player1 = sd.state.players[0].login == sd.l1.login ? sd.state.players[0] : sd.state.players[1];
        gm::Player& player2 = sd.state.players[1].login == sd.l1.login ? sd.state.players[0] : sd.state.players[1]; 

        StringResponse response = Walk(socket, serializer, {player1.posX + 1, player1.posY}, sd.l1.token, sd.sid);
        CheckStringResponse(response, {
            .body=SESSION_FINISHED,
            .res=http::status::bad_request
        });

        response = Walk(socket, serializer, {player2.posX + 1, player2.posY}, sd.l1.token, sd.sid);
        CheckStringResponse(response, {
            .body=SESSION_FINISHED,
            .res=http::status::bad_request
        });
    }
}