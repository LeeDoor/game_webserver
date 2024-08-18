#include "socket_functions.hpp"
#include "api_functions.hpp"

#include "session.hpp"

#include "json_type_converter.hpp"
#include "type_serializer.hpp"

#define BOMB_TICKS_DEFAULT 3

TEST_CASE("ApiPlaceBomb", "[api][game][move][place_bomb]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    
    std::string WRONG_MOVE = serializer::SerializeError("wrong_move", "player cant make such move");
    std::string NOT_YOUR_MOVE = serializer::SerializeError("not_your_move", "the opponent's move is now");
    std::string ACCESS_DENIED = serializer::SerializeError("access_denied", "you have no access to do this action");
    std::string NO_SUCH_SESSION = serializer::SerializeError("wrong_sessionId", "no session with such sessionId");
    std::string WRONG_BODY_DATA = serializer::SerializeError("body_data_error", "wrong body data");

    SECTION("custom_room"){
        SessionData sd = CreateNewMatch(socket);
        gm::State state = sd.state;
        state.map_size = {3,3};
        state.now_turn = sd.l1.login;
        state.players[0].posX = 0;
        state.players[0].posY = 1;
        state.players[1].posX = 2;
        state.players[1].posY = 1;
        state.terrain = {gm::Obstacle{1,1,gm::Obstacle::Type::Wall}};
        SetStateSuccess(socket, state, sd.sid);
        StringResponse response;
        
        // * * * * * * *
        // *   *   *   *
        // * * * * * * *
        // * A * W * B *
        // * * * * * * *
        // *   *   *   *
        // * * * * * * *

        SECTION("events_one_bomb"){
            INFO("A cant place the bomb on the wall");
            response = PlaceBomb(socket, {1,1}, sd.l1.token, sd.sid);
            CheckStringResponse(response,{
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });

            INFO("A cant place the bomb on the far-away cells");
            response = PlaceBomb(socket, {2,0}, sd.l1.token, sd.sid);
            CheckStringResponse(response,{
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });
            response = PlaceBomb(socket, {2,1}, sd.l1.token, sd.sid);
            CheckStringResponse(response,{
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });
            response = PlaceBomb(socket, {2,2}, sd.l1.token, sd.sid);
            CheckStringResponse(response,{
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });
            INFO("A can place the bomb above the wall");
            PlaceBombSuccess(socket, {1,0}, sd.l1.token, sd.sid);

            
            INFO("event list contains bomb place and bomb tick");
            {
                StringResponse events_resp = SessionStateChange(socket, sd.l1.token, sd.sid, 1);
                nlohmann::json j = nlohmann::json::parse(events_resp.body());
                INFO(j.dump());
                REQUIRE(j.is_array());
                REQUIRE(j.size() == 2);
                CHECK(j[0]["event_type"] == "player_place_bomb");
                CHECK(j[0]["actor_id"] == 0);
                CHECK(j[0]["move_number"] == 1);
                CHECK(j[0]["data"]["place"]["posX"] == 1);
                CHECK(j[0]["data"]["place"]["posY"] == 0);
                CHECK(j[0]["data"]["new_object"]["actor_id"] == 2);
                CHECK(j[0]["data"]["ticks_left"] == BOMB_TICKS_DEFAULT);

                CHECK(j[1]["event_type"] == "bomb_ticking");
                CHECK(j[1]["actor_id"] == 2);
                CHECK(j[1]["move_number"] == 1);
            }

            state = SessionStateSuccess(socket, sd.l1.token, sd.sid);
            INFO("state contains one bomb");
            REQUIRE(state.objects.size() == 1);
            CHECK(state.objects[0]->posX == 1);
            CHECK(state.objects[0]->posY == 0);
            {
                gm::Bomb::Ptr bomb = dynamic_pointer_cast<gm::Bomb>(state.objects[0]);
                REQUIRE(bomb);
                CHECK(bomb->ticks_left == BOMB_TICKS_DEFAULT - 1);
                if(bomb->ticks_left != 2){
                    WARN("!!!!!bomb ticks are changed from 3 to " << bomb->ticks_left + 1 << ". tests deprecated.!!!!!");
                }
            }

            
            // * * * * * * *
            // *   * @ *   *
            // * * * * * * *
            // * A * W * B *
            // * * * * * * *
            // *   *   *   *
            // * * * * * * *
            // @ - is a bomb

            INFO("B cant place bombs on invalid cells");
            response = PlaceBomb(socket, {0,0}, sd.l2.token, sd.sid);
            CheckStringResponse(response,{
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });
            response = PlaceBomb(socket, {0,1}, sd.l2.token, sd.sid);
            CheckStringResponse(response,{
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });
            response = PlaceBomb(socket, {0, 2}, sd.l2.token, sd.sid);
            CheckStringResponse(response,{
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });
            response = PlaceBomb(socket, {1,1}, sd.l2.token, sd.sid);
            CheckStringResponse(response,{
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });
            INFO("B walks below");
            WalkSuccess(socket, {2,2}, sd.l2.token, sd.sid);

            INFO("A walks below");
            WalkSuccess(socket, {0,2}, sd.l1.token, sd.sid);
            
            // * * * * * * *
            // *   *   *   *
            // * * * * * * *
            // *   * W *   *
            // * * * * * * *
            // * A *   * B *
            // * * * * * * *


            INFO("event list contains B-walk, tick, A-walk, explosion");
            {
                StringResponse events_resp = SessionStateChange(socket, sd.l1.token, sd.sid, 2);
                nlohmann::json j = nlohmann::json::parse(events_resp.body());
                REQUIRE(j.is_array());
                REQUIRE(j.size() == 4);
                CHECK(j[0]["event_type"] == "player_walk");
                CHECK(j[0]["actor_id"] == 1);
                CHECK(j[0]["move_number"] == 2);

                CHECK(j[1]["event_type"] == "bomb_ticking");
                CHECK(j[1]["actor_id"] == 2);
                CHECK(j[1]["move_number"] == 2);

                CHECK(j[2]["event_type"] == "player_walk");
                CHECK(j[2]["actor_id"] == 0);
                CHECK(j[2]["move_number"] == 3);

                CHECK(j[3]["event_type"] == "bomb_explode");
                CHECK(j[3]["actor_id"] == 2);
                CHECK(j[3]["move_number"] == 3);
            }
            INFO("bomb exploded on state");
            state = SessionStateSuccess(socket, sd.l1.token, sd.sid);
            REQUIRE(state.objects.size() == 0);
        }

        SECTION("events_three_bombs"){
            INFO("A can place the bomb above the wall");
            PlaceBombSuccess(socket, {1,0}, sd.l1.token, sd.sid);

            INFO("bomb placed");
            state = SessionStateSuccess(socket, sd.l1.token, sd.sid);
            REQUIRE(state.objects.size() == 1);

            INFO("B can place the bomb below the wall");
            PlaceBombSuccess(socket, {1,2}, sd.l2.token, sd.sid);

            INFO("bomb placed");
            state = SessionStateSuccess(socket, sd.l1.token, sd.sid);
            REQUIRE(state.objects.size() == 2);

            INFO("A can place the bomb below himself");
            PlaceBombSuccess(socket, {0,2}, sd.l1.token, sd.sid);

            INFO("event list contains A-place, tick, B-place, tick, tick, A-place, explode, win (8 events)");
            {
                StringResponse events_resp = SessionStateChange(socket, sd.l1.token, sd.sid, 1);
                nlohmann::json j = nlohmann::json::parse(events_resp.body());
                INFO(j.dump());
                REQUIRE(j.is_array());
                REQUIRE(j.size() == 8);
                CHECK(j[0]["event_type"] == "player_place_bomb");
                CHECK(j[0]["actor_id"] == 0);
                CHECK(j[0]["move_number"] == 1);

                CHECK(j[1]["event_type"] == "bomb_ticking");
                CHECK(j[1]["actor_id"] == 2);
                CHECK(j[1]["move_number"] == 1);

                CHECK(j[2]["event_type"] == "player_place_bomb");
                CHECK(j[2]["actor_id"] == 1);
                CHECK(j[2]["move_number"] == 2);

                CHECK(j[3]["event_type"] == "bomb_ticking");
                CHECK(j[3]["actor_id"] == 2);
                CHECK(j[3]["move_number"] == 2);

                CHECK(j[4]["event_type"] == "bomb_ticking");
                CHECK(j[4]["actor_id"] == 3);
                CHECK(j[4]["move_number"] == 2);

                CHECK(j[5]["event_type"] == "player_place_bomb");
                CHECK(j[5]["actor_id"] == 0);
                CHECK(j[5]["move_number"] == 3);

                CHECK(j[6]["event_type"] == "bomb_explode");
                CHECK(j[6]["actor_id"] == 2);
                CHECK(j[6]["move_number"] == 3);

                CHECK(j[7]["event_type"] == "player_won");
                CHECK(j[7]["actor_id"] <= 1);
                CHECK(j[7]["actor_id"] >= 0);
                CHECK(j[7]["move_number"] == 3);
            }
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
                auto response = PlaceBomb(socket, wd, ld.token, sid);
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
        auto response = PlaceBomb(socket, {0,0}, noname.token, sid);
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

        auto response = PlaceBomb(socket, {0,0}, ld1.token, "ABOBUS");
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

        http::response<http::string_body> response = Move(socket, "{\"move_type\":\"place_bomb\"}", ld.token, sid);
        CheckStringResponse(response, 
            {
                .body = WRONG_BODY_DATA,
                .res = http::status::bad_request
            });

        nlohmann::json obj(gm::PlaceData{0,0});
        obj["move_type"] = "place_bomb";
        obj["posX"] = "nigger";

        response = Move(socket, obj.dump(), ld.token, sid);
        CheckStringResponse(response, 
            {
                .body = WRONG_BODY_DATA,
                .res = http::status::bad_request
            });
    }
}