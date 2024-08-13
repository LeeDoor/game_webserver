#include "socket_functions.hpp"
#include "api_functions.hpp"

#include "session.hpp"

#include "json_type_converter.hpp"
#include "type_serializer.hpp"


bool ValidCell(const gm::State& state, unsigned x, unsigned y){
    auto& terrain = state.terrain;
    
    auto it = std::find_if(terrain.begin(), terrain.end(), 
        [&](const gm::Obstacle& o){return o.posX == x && o.posY == y;});
    return it == terrain.end();
}

TEST_CASE("ApiMove", "[api][game][move]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    
    std::string WRONG_MOVE = serializer::SerializeError("wrong_move", "player cant make such move");
    std::string NOT_YOUR_MOVE = serializer::SerializeError("not_your_move", "the opponent's move is now");
    std::string ACCESS_DENIED = serializer::SerializeError("access_denied", "you have no access to do this action");
    std::string NO_SUCH_SESSION = serializer::SerializeError("wrong_sessionId", "no session with such sessionId");
    std::string WRONG_BODY_DATA = serializer::SerializeError("body_data_error", "wrong body data");

    SECTION("success"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        std::optional<um::Login> prev_turn = std::nullopt;
        std::optional<gm::PlaceData> new_wd = std::nullopt;
        for(int i = 0; i < 10; ++i){
            gm::State state = SessionStateSuccess(socket, ld1.token, sid);
            REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

            um::Login& now_turn = state.now_turn;
            LoginData& ld = ld1.login == now_turn ? ld1 : ld2;
            gm::Player& player = state.players[0].login == now_turn ? state.players[0] : state.players[1];

            if(prev_turn)
                CHECK(*prev_turn != now_turn);

            if(new_wd){
                gm::Player& waiting_player = state.players[1].login == now_turn ? state.players[0] : state.players[1];
                CHECK(waiting_player.posX == new_wd->posX);
                CHECK(waiting_player.posY == new_wd->posY);
            }

            std::vector<gm::PlaceData> wds{
                {player.posX + 1, player.posY},
                {player.posX, player.posY + 1},
                {player.posX - 1, player.posY},
                {player.posX, player.posY - 1},
            }; 
            bool moved = false;
            for(auto& wd : wds){
                if (ValidCell(state, wd.posX, wd.posY)){
                    WalkSuccess(socket, wd, ld.token, sid);
                    moved = true;
                    break;
                }
            }
            if (!moved) break;
            prev_turn = std::string(now_turn);
        }
    }

    SECTION("wrong_move"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        
        gm::State state = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        um::Login& now_turn = state.now_turn;
        LoginData& ld = ld1.login == now_turn ? ld1 : ld2;
        gm::Player& player = state.players[0].login == now_turn ? state.players[0] : state.players[1];

        std::vector<StringResponse> responses = {
            Walk(socket, {player.posX + 2, player.posY + 3}, ld.token, sid),
            Walk(socket, {player.posX + 1, player.posY + 3}, ld.token, sid),
            Walk(socket, {player.posX + 1, player.posY + 1}, ld.token, sid),
            Walk(socket, {player.posX + 2, player.posY}, ld.token, sid),
            Walk(socket, {player.posX + 3, player.posY}, ld.token, sid),
            Walk(socket, {player.posX -20, player.posY}, ld.token, sid),
            Walk(socket, {player.posX -2, player.posY}, ld.token, sid),
            Walk(socket, {player.posX, player.posY + 2}, ld.token, sid),
            Walk(socket, {player.posX, player.posY + 3}, ld.token, sid),
            Walk(socket, {player.posX, player.posY - 20}, ld.token, sid),
            Walk(socket, {player.posX, player.posY - 2}, ld.token, sid),
            Walk(socket, {player.posX + 1, player.posY - 1}, ld.token, sid),
        };
        for(auto& response : responses){
            CheckStringResponse(response,{
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });
        }
    }

    SECTION("not_your_move"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        
        gm::State state = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        um::Login& now_turn = state.now_turn;
        LoginData& ld = ld1.login == now_turn ? ld2 : ld1;
        gm::Player& player = state.players[0].login == now_turn ? state.players[1] : state.players[0];

        std::vector<gm::PlaceData> wds{
            {player.posX + 1, player.posY},
            {player.posX, player.posY + 1},
            {player.posX - 1, player.posY},
            {player.posX, player.posY - 1},
        }; 
        for(auto& wd : wds){
            if (ValidCell(state, wd.posX, wd.posY)){
                auto response = Walk(socket, wd, ld.token, sid);
                CheckStringResponse(response,{
                    .body = NOT_YOUR_MOVE,
                    .res = http::status::bad_request
                });
            }
        }
    }

    SECTION("access_denied"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        
        gm::State state = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        LoginData noname = EnqueueNewPlayer(socket);
        auto response = Walk(socket, {0,0}, noname.token, sid);
        CheckStringResponse(response,{
            .body = ACCESS_DENIED,
            .res = http::status::bad_request
        });
    }

    SECTION("no_such_session"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        
        gm::State state = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        auto response = Walk(socket, {0,0}, ld1.token, "ABOBUS");
        CheckStringResponse(response,{
            .body = NO_SUCH_SESSION,
            .res = http::status::bad_request
        });
    }

    SECTION("wrong_body_data"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        
        gm::State state = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        um::Login& now_turn = state.now_turn;
        LoginData& ld = ld1.login == now_turn ? ld2 : ld1;

        http::response<http::string_body> response = Move(socket, "{}", ld.token, sid);
        CheckStringResponse(response, 
            {
                .body = WRONG_BODY_DATA,
                .res = http::status::bad_request
            });

        nlohmann::json obj(gm::PlaceData{0,0});
        obj["move_type"] = "walk";
        obj["posX"] = "nigger";

        response = Move(socket, obj.dump(), ld.token, sid);
        CheckStringResponse(response, 
            {
                .body = WRONG_BODY_DATA,
                .res = http::status::bad_request
            });
    }
}

TEST_CASE("ApiResign", "[api][game][resign]"){
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
        INFO("resign is successful");
        sm::PublicSessionData psd = PublicSessionDataSuccess(socket, sd.sid, sd.l1.token);
        CHECK(psd == PublicSessionDataSuccess(socket, sd.sid, sd.l2.token));
        CHECK(*psd.winner != *psd.loser);
        CHECK(*psd.loser == ld.login);
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
        
        gm::Player& player1 = sd.state.players[0].login == sd.l1.login ? sd.state.players[0] : sd.state.players[1];
        gm::Player& player2 = sd.state.players[1].login == sd.l1.login ? sd.state.players[0] : sd.state.players[1]; 

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