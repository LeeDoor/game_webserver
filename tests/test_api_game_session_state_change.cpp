#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "serializer_session.hpp"
#include "serializer_game.hpp"
#include "session.hpp"
#include <thread>
#include "nlohmann/json.hpp"
#include "type_serializer.hpp"

using json = nlohmann::json;

void ValidateEvents(json j){
    REQUIRE(j.is_array());
    nlohmann::json event;
    for(int i = 0; i < j.size(); ++i){
        REQUIRE_NOTHROW(event = j[i]);
        REQUIRE(event.contains("actor_id"));
        REQUIRE(event.contains("data"));
        REQUIRE(event.contains("event_type"));
        REQUIRE(event.contains("move_number"));
    }
}
void ValidateEvents(json j, gm::ActorId actor_id, std::string event_type, int move_number, int id = 0){
    REQUIRE(j.is_array());
    CHECK(j.size() > id);
    nlohmann::json event;
    REQUIRE_NOTHROW(event = j[id]);
    REQUIRE(event.contains("actor_id"));
    CHECK(event.at("actor_id") == 0); // now it means that first player moved. feel free change it in the future
    REQUIRE(event.contains("data"));
    REQUIRE(event.contains("event_type"));
    CHECK(event.at("event_type") == "player_walk"); 
    REQUIRE(event.contains("move_number"));
    CHECK(event.at("move_number") == 1); 
}

TEST_CASE("ApiSessionStateChange", "[api][game][session_state_change][long_poll]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);

    net::io_context ioc2;
    tcp::socket socket2{ioc2};
    ConnectSocket(ioc2, socket2);

    net::io_context ioc3;
    tcp::socket socket3{ioc3};
    ConnectSocket(ioc3, socket3);

    
    std::string SESSION_FINISHED = serializer::SerializeError("session_finished", "session is finished");
    std::string WRONG_SESSIONID = serializer::SerializeError("wrong_sessionId", "no session with such sessionId");
    std::string URL_PARAMETERS_ERROR = serializer::SerializeError("url_parameters_error", "this api function requires url parameters");
    std::string POLL_CLOSED = serializer::SerializeError("poll_closed", "SessionStateNotifier poll replaced by other");

    SECTION("success"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        
        gm::State state = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        std::promise<void> promise1, promise2;
        std::future<void> future1 = promise1.get_future(),
                    future2 = promise2.get_future();
        StringResponse response1, response2;
        std::thread thread1 ([&]{
            response1 = SessionStateChange(socket, ld1.token, sid, state.move_number);
            promise1.set_value();
        });
        std::thread thread2 ([&]{
            response2 = SessionStateChange(socket2, ld2.token, sid, state.move_number);
            promise2.set_value();
        });

        um::Login& now_turn = state.now_turn;
        LoginData& ld = ld1.login == now_turn ? ld1 : ld2;
        gm::Player& player = *(state.players.front()->login == now_turn ? state.players.front() : state.players.back());

        INFO(player.posX << " " << player.posY);

        WalkSuccess(socket3, {player.posX + 1, player.posY}, ld.token, sid);

        future1.get();
        future2.get();

        CheckStringResponse(response1, {
            .res = http::status::ok
        });
        CheckStringResponse(response2, {
            .res = http::status::ok
        });
        using json = nlohmann::json;
        json j1 = json::parse(response1.body()), j2 = json::parse(response2.body());
        INFO(j1.dump());
        INFO(j2.dump());
        CHECK(j1 == j2);

        ValidateEvents(j1, 0, "player_walk", 1);

        thread1.join();
        thread2.join();
    }
    SECTION("poll_closed"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        
        gm::State state = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        std::promise<void> p1, p2, p3;
        std::future<void> f1 = p1.get_future(), f2 = p2.get_future(), f3 = p3.get_future();

        StringResponse response1, response2;

        std::thread th1 ([&]{
            response1 = SessionStateChange(socket, ld1.token, sid, state.move_number);
            p1.set_value();
        });
        std::thread th2 ([&]{
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            response2 = SessionStateChange(socket2, ld1.token, sid, state.move_number);
            p2.set_value();
        });
        f1.wait();
        CheckStringResponse(response1, {
            .body = POLL_CLOSED,
            .res = http::status::conflict
        });
        std::thread th3 ([&]{
            response1 = SessionStateChange(socket, ld2.token, sid, state.move_number);
            p3.set_value();
        });

        um::Login& now_turn = state.now_turn;
        LoginData& ld = ld1.login == now_turn ? ld1 : ld2;
        gm::Player& player = *(state.players.front()->login == now_turn ? state.players.front() : state.players.back());
        WalkSuccess(socket3, {player.posX + 1, player.posY}, ld.token, sid);

        f2.wait();
        f3.wait();

        CheckStringResponse(response1,{
            .body = response2.body(),
            .res = http::status::ok
        });

        CheckStringResponse(response2,{
            .res = http::status::ok
        });
        
        th1.join();
        th2.join();
        th3.join();
    }
    SECTION("wrong_sessionId"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        
        gm::State state = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        StringResponse response = SessionStateChange(socket2, ld2.token, "ABOBUSSS", state.move_number);
        CheckStringResponse(response, {
            .body = WRONG_SESSIONID,
            .res = http::status::bad_request
        });
    }
    SECTION("url_parameters_error"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        
        gm::State state = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        StringResponse response;
        http::request<http::string_body> request{http::verb::get, SESSION_STATE_CHANGE_API, 11};

        SetAuthorizationHeader(request, ld1.token);
        http::write(socket, request);
        beast::flat_buffer buffer;
        http::read(socket, buffer, response);

        CheckStringResponse(response, {
            .body = URL_PARAMETERS_ERROR,
            .res = http::status::unprocessable_entity
        });
    }

    SECTION("multiple_moves_passed"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token));
        
        gm::State state = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        um::Login& now_turn = state.now_turn;
        for(int i = 0; i < 4; ++i){
            state = SessionStateSuccess(socket, ld1.token, sid);
            um::Login& now_turn = state.now_turn;
            LoginData& ld = ld1.login == now_turn ? ld1 : ld2;
            gm::Player& player = * (state.players.front()->login == now_turn ? state.players.front() : state.players.back());
            WalkSuccess(socket3, {player.posX + 1, player.posY}, ld.token, sid);
        }

        auto response = SessionStateChange(socket, ld1.token, sid, 1);
        INFO(response.body());
        json j = json::parse(response.body());
        ValidateEvents(j);
        bool res = j[0].at("actor_id") ==  j[2].at("actor_id") && j[1].at("actor_id") ==  j[3].at("actor_id") && j[0].at("actor_id") !=  j[1].at("actor_id");
        INFO("!!! checking changing actor_id");
        CHECK(res);
        res = j[0].at("move_number") == 1 && j[1].at("move_number") == 2 && j[2].at("move_number") == 3 && j[3].at("move_number") == 4;
        INFO("!!! checking changing move_number");
        CHECK(res);

        response = SessionStateChange(socket, ld2.token, sid, 3);
        INFO(response.body());
        j = json::parse(response.body());
        ValidateEvents(j);
        REQUIRE(j.size() == 2);
        res = j[0].at("actor_id") !=  j[1].at("actor_id");
        INFO("!!! checking changing actor_id");
        CHECK(res);
        res = j[0].at("move_number") == 3 && j[1].at("move_number") == 4;
        INFO("!!! checking changing move_number");
        CHECK(res);
    }
    SECTION("after_end") {
        SessionData sd = CreateNewMatch(socket);
        StringResponse response;
        std::thread t ([&]{
            response = SessionStateChange(socket, sd.l1.token, sd.sid, 1);
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ResignSuccess(socket2, sd.l1.token, sd.sid);
        // get as first. polled before match ended. we should get latest data
        t.join();
        CheckStringResponse(response, {
            .res = http::status::ok
        });
        json j = json::parse(response.body());
        CHECK(j.is_array());
        CHECK(j.size() == 2);
        // get as first. already got latest data. should throw
        response = SessionStateChange(socket, sd.l1.token, sd.sid, 1);
        CheckStringResponse(response, {
            .body = SESSION_FINISHED,
            .res = http::status::bad_request
        });

        // get as second. didnt poll before match ended. should get latest data
        response = SessionStateChange(socket, sd.l2.token, sd.sid, 1);
        CheckStringResponse(response, {
            .res = http::status::ok
        });
        CHECK(j == json::parse(response.body()));
    }
}