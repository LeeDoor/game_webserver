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

TEST_CASE("ApiMove", "[api][game][move][walk]"){
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
    SECTION("in_prepared_room_walless"){
        SessionData sd = CreateNewMatch(socket);
        gm::State state = sd.state;
        state.map_size = {2,2};
        state.now_turn = sd.l1.login;
        state.players[0].posX = 0;
        state.players[0].posY = 0;
        state.players[1].posX = 1;
        state.players[1].posY = 0;
        SetStateSuccess(socket, state, sd.sid);
        // XXXXXXXXXX
        //Y * * * * *
        //Y * A * B *
        //Y * * * * *
        //Y *   *   *
        //Y * * * * *

        StringResponse response;
        INFO("A cant walk into the enemy");
        response = Walk(socket, {1, 0}, sd.l1.token, sd.sid);
        CheckStringResponse(response,{
            .body=WRONG_MOVE,
            .res=http::status::bad_request
        });

        INFO("A can walk at cell below");
        WalkSuccess(socket, {0, 1}, sd.l1.token, sd.sid);
        
        // * * * * *
        // *   * B *
        // * * * * *
        // * A *   *
        // * * * * *

        INFO("B cant walk right");
        response = Walk(socket, {0, 2}, sd.l2.token, sd.sid);
        CheckStringResponse(response,{
            .body=WRONG_MOVE,
            .res=http::status::bad_request
        });

        INFO("B can walk left, where opponent just was");
        WalkSuccess(socket, {0, 0}, sd.l2.token, sd.sid);

        // * * * * *
        // * B *   *
        // * * * * *
        // * A *   *
        // * * * * *

        INFO("A cant go down");
        response = Walk(socket, {2, 0}, sd.l1.token, sd.sid);
        CheckStringResponse(response,{
            .body=WRONG_MOVE,
            .res=http::status::bad_request
        });

        INFO("A cant go above on B");
        response = Walk(socket, {0, 0}, sd.l1.token, sd.sid);
        CheckStringResponse(response,{
            .body=WRONG_MOVE,
            .res=http::status::bad_request
        });

        INFO("A can walk right");
        WalkSuccess(socket, {1, 1}, sd.l1.token, sd.sid);
    }
}