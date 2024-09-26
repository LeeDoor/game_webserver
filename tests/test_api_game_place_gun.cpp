#include "socket_functions.hpp"
#include "api_functions.hpp"

#include "session.hpp"

#include "json_type_converter.hpp"
#include "type_serializer.hpp"

#define GUN_SHOTS_DEFAULT 3
#define GUN_TICKS_DEFAULT 3

TEST_CASE("ApiPlaceGun", "[api][game][move][place_gun]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    
    std::string WRONG_MOVE = serializer::SerializeError("wrong_move", "player cant make such move");
    std::string NOT_YOUR_MOVE = serializer::SerializeError("not_your_move", "the opponent's move is now");
    std::string ACCESS_DENIED = serializer::SerializeError("access_denied", "you have no access to do this action");
    std::string NO_SUCH_SESSION = serializer::SerializeError("wrong_sessionId", "no session with such sessionId");
    std::string WRONG_BODY_DATA = serializer::SerializeError("body_data_error", "wrong body data");

    SECTION("custom room"){
        SessionData sd = CreateNewMatch(socket);
        gm::State& state = sd.state;
        state.map_size = {3,3};
        state.now_turn = sd.l1.login;
        state.players.front()->position.x = 0;
        state.players.front()->position.y = 1;
        state.players.back()->position.x = 2;
        state.players.back()->position.y = 1;
        state.terrain = {std::make_shared<gm::Obstacle>(gm::Obstacle{{1,1},gm::Obstacle::Type::Wall})};
        SetStateSuccess(socket, state, sd.sid);
        StringResponse response;
        
        // * * * * * * *
        // *   *   *   *
        // * * * * * * *
        // * A * W * B *
        // * * * * * * *
        // *   *   *   *
        // * * * * * * *

        INFO("cant place gun on wall");
        for(int i = 0; i < 4; ++i){
            response = PlaceGun(socket, {1,1}, static_cast<gm::Direction>(i), sd.l1.token, sd.sid);
            CheckStringResponse(response, {
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });
        }

        INFO("cant place gun on yourself");
        for(int i = 0; i < 4; ++i){
            response = PlaceGun(socket, {0,1}, static_cast<gm::Direction>(i), sd.l1.token, sd.sid);
            CheckStringResponse(response, {
                .body = WRONG_MOVE,
                .res = http::status::bad_request
            });
        }
        
        INFO("can place gun at the top, looking right");
        PlaceGunSuccess(socket, {0,0}, gm::Direction::Right, sd.l1.token, sd.sid); // 1

        gm::ActorId gunAI, bulletAI;
        INFO("event list contains gun place and gun waiting");
        {
            StringResponse events_resp = SessionStateChange(socket, sd.l1.token, sd.sid, 1);
            nlohmann::json j = nlohmann::json::parse(events_resp.body());
            INFO(j.dump());
            REQUIRE(j.is_array());
            REQUIRE(j.size() == 2);
            CHECK(j[0]["event"] == "player_place_gun");
            CHECK(j[0]["actor_id"] == 0);
            CHECK(j[0]["move_number"] == 1);
            CHECK(j[0]["position"]["x"] == 0);
            CHECK(j[0]["position"]["y"] == 0);
            CHECK(j[0]["direction"] == "right");
            REQUIRE_NOTHROW(gunAI = j[0]["new_actor_id"].get<gm::ActorId>());
            CHECK(gunAI != 0); 
            CHECK(gunAI != 1);

            CHECK(j[1]["event"] == "gun_waiting");
            CHECK(j[1]["actor_id"] == gunAI);
            CHECK(j[1]["move_number"] == 1);
        }

        INFO("state contains one gun");
        state = SessionStateSuccess(socket, sd.l1.token, sd.sid);
        REQUIRE(state.objects.size() == 1);
        auto iter = state.objects.begin();
        {
            gm::Gun::Ptr gun = dynamic_pointer_cast<gm::Gun>(*iter);
            REQUIRE(gun);
            CHECK(gun->position.x == 0);
            CHECK(gun->position.y == 0);
            CHECK(gun->direction == gm::Direction::Right);
            CHECK(gun->ticks_to_shot == GUN_TICKS_DEFAULT - 1);
            CHECK(gun->shots_left == GUN_SHOTS_DEFAULT);
            if(gun->ticks_to_shot != 2){
                WARN("!!!!!gun ticks are changed from 3 to " << gun->ticks_to_shot + 1 << ". tests deprecated.!!!!!");
            }
        }
        
        // * * * * * * *
        // * > *   *   *
        // * * * * * * *
        // * A * W * B *
        // * * * * * * *
        // *   *   *   *
        // * * * * * * *
        // > - direction of gun

        INFO("A & B walks below");
        WalkSuccess(socket, {2,2}, sd.l2.token, sd.sid); // 2
        WalkSuccess(socket, {0,2}, sd.l1.token, sd.sid); // 3

        INFO("walk, tick, walk, shoot, bullet_fly");
        {
            StringResponse events_resp = SessionStateChange(socket, sd.l1.token, sd.sid, 2);
            nlohmann::json j = nlohmann::json::parse(events_resp.body());
            INFO(j.dump());
            REQUIRE(j.is_array());
            REQUIRE(j.size() == 5);
            CHECK(j[0]["event"] == "player_walk");

            CHECK(j[1]["event"] == "gun_waiting");
            CHECK(j[1]["actor_id"] == gunAI);

            CHECK(j[2]["event"] == "player_walk");

            CHECK(j[3]["event"] == "gun_shot");
            CHECK(j[3]["position"]["x"] == 0);
            CHECK(j[3]["position"]["y"] == 0);
            CHECK(j[3]["direction"] == "right");
            CHECK(j[3]["actor_id"] == gunAI);
            REQUIRE_NOTHROW(bulletAI = j[3]["new_actor_id"].get<gm::ActorId>());

            CHECK(j[4]["event"] == "bullet_fly");
            CHECK(j[4]["actor_id"] == bulletAI);
        }

        INFO("state contains one gun & bullet");
        state = SessionStateSuccess(socket, sd.l1.token, sd.sid);
        REQUIRE(state.objects.size() == 2);
        iter = state.objects.begin();
        {
            gm::Gun::Ptr gun = dynamic_pointer_cast<gm::Gun>(*iter);
            REQUIRE(gun);
            CHECK(gun->ticks_to_shot == GUN_TICKS_DEFAULT);
            CHECK(gun->shots_left == GUN_SHOTS_DEFAULT - 1);

            std::advance(iter, 1);

            gm::Bullet::Ptr bullet = dynamic_pointer_cast<gm::Bullet>(*iter);
            REQUIRE(bullet);
            CHECK(bullet->position.x == 1);
            CHECK(bullet->position.y == 0);
            CHECK(bullet->direction == gun->direction);
        }
        
        // * * * * * * *
        // * > * @ *   *
        // * * * * * * *
        // *   * W *   *
        // * * * * * * *
        // * A *   * B *
        // * * * * * * *

        INFO("A & B walks up");
        WalkSuccess(socket, {2,1}, sd.l2.token, sd.sid); // 4
        WalkSuccess(socket, {0,1}, sd.l1.token, sd.sid); // 5

        INFO("walk, tick, fly, walk, tick, destroy");
        {
            StringResponse events_resp = SessionStateChange(socket, sd.l1.token, sd.sid, 4);
            nlohmann::json j = nlohmann::json::parse(events_resp.body());
            INFO(j.dump());
            REQUIRE(j.is_array());
            REQUIRE(j.size() == 6);
            CHECK(j[0]["event"] == "player_walk");

            CHECK(j[1]["event"] == "gun_waiting");
            CHECK(j[1]["actor_id"] == gunAI);

            CHECK(j[2]["event"] == "bullet_fly");
            CHECK(j[2]["actor_id"] == bulletAI);

            CHECK(j[3]["event"] == "player_walk");

            CHECK(j[4]["event"] == "gun_waiting");
            CHECK(j[4]["actor_id"] == gunAI);

            CHECK(j[5]["event"] == "bullet_destroy");
            CHECK(j[5]["actor_id"] == bulletAI);
        }

        INFO("state contains one gun and no bullets");
        state = SessionStateSuccess(socket, sd.l1.token, sd.sid);
        REQUIRE(state.objects.size() == 1);
        iter = state.objects.begin();
        {
            gm::Gun::Ptr gun = dynamic_pointer_cast<gm::Gun>(*iter);
            REQUIRE(gun);
            CHECK(gun->ticks_to_shot == 1);
            CHECK(gun->shots_left == GUN_SHOTS_DEFAULT - 1);
        }
        
        // * * * * * * *
        // * > *   *   *
        // * * * * * * *
        // * A * W * B *
        // * * * * * * *
        // *   *   *   *
        // * * * * * * *

        INFO("B - up, A - below, B gets shot");
        WalkSuccess(socket, {2,0}, sd.l2.token, sd.sid); //6 
        response = Walk(socket, {0,0}, sd.l1.token, sd.sid);
        CheckStringResponse(response, {
            .body = WRONG_MOVE,
            .res = http::status::bad_request
        });
        WalkSuccess(socket, {0,2}, sd.l1.token, sd.sid); // 7

        INFO("walk, shot, fly, walk, tick, destroy, player_won");
        {
            StringResponse events_resp = SessionStateChange(socket, sd.l1.token, sd.sid, 6);
            nlohmann::json j = nlohmann::json::parse(events_resp.body());
            INFO(j.dump());
            REQUIRE(j.is_array());
            REQUIRE(j.size() == 8);
            CHECK(j[0]["event"] == "player_walk");

            CHECK(j[1]["event"] == "gun_shot");
            CHECK(j[1]["position"]["x"] == 0);
            CHECK(j[1]["position"]["y"] == 0);
            CHECK(j[1]["direction"] == "right");
            CHECK(j[1]["actor_id"] == gunAI);
            REQUIRE_NOTHROW(bulletAI = j[1]["new_actor_id"].get<gm::ActorId>());

            CHECK(j[2]["event"] == "bullet_fly");
            CHECK(j[2]["actor_id"] == bulletAI);

            CHECK(j[3]["event"] == "player_walk");

            CHECK(j[4]["event"] == "gun_waiting");
            CHECK(j[4]["actor_id"] == gunAI);
            
            CHECK(j[5]["event"] == "bullet_interaction");
            CHECK(j[5]["actor_id"] == bulletAI);
            CHECK(j[5]["interacted_actor_id"] == 1);
            CHECK(j[5]["happened"].is_array());
            CHECK(j[5]["happened"].size() == 1);
            CHECK(j[5]["happened"][0]["event"] == "player_dead");
            CHECK(j[5]["happened"][0]["move_number"] == 7);
            CHECK(j[5]["happened"][0]["actor_id"] == 1);

            CHECK(j[6]["event"] == "bullet_destroy");
            CHECK(j[6]["actor_id"] == bulletAI);

            CHECK(j[7]["event"] == "player_won");
            CHECK(j[7]["actor_id"] == 0);
        }

        sm::PublicSessionData psd = PublicSessionDataSuccess(socket, sd.sid, sd.l1.token);
        CHECK(*psd.winner != *psd.loser);
        CHECK(*psd.loser == sd.l2.login);
        CHECK(*psd.winner == sd.l1.login);
    }
}