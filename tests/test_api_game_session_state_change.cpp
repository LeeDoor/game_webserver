#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "json_serializer.hpp"
#include "session.hpp"
#include <thread>
using namespace serializer;

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

    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string WRONG_SESSIONID = serializer->SerializeError("wrong_sessionId", "no session with such sessionId");
    std::string URL_PARAMETERS_ERROR = serializer->SerializeError("url_parameters_error", "this api function requires url parameters");
    std::string POLL_CLOSED = serializer->SerializeError("poll_closed", "SessionStateNotifier poll replaced by other");

    SECTION("success"){
        if(MMQueueSuccess(socket, serializer).size() == 1)
            EnqueueNewPlayer(socket, serializer);
        LoginData ld1 = EnqueueNewPlayer(socket, serializer);
        LoginData ld2 = EnqueueNewPlayer(socket, serializer);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token, serializer);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token, serializer));
        
        gm::State state = SessionStateSuccess(socket, serializer, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, serializer, ld2.token, sid));

        std::promise<void> promise1, promise2;
        std::future<void> future1 = promise1.get_future(),
                    future2 = promise2.get_future();
        StringResponse response1, response2;
        std::thread thread1 ([&]{
            response1 = SessionStateChange(socket, ld1.token, sid);
            promise1.set_value();
        });
        std::thread thread2 ([&]{
            response2 = SessionStateChange(socket2, ld2.token, sid);
            promise2.set_value();
        });

        dm::Login& now_turn = state.now_turn;
        LoginData& ld = ld1.login == now_turn ? ld1 : ld2;
        gm::Player& player = state.players[0].login == now_turn ? state.players[0] : state.players[1];

        Walk(socket3, serializer, {player.posX + 1, player.posY}, ld.token, sid);

        future1.get();
        future2.get();

        CheckStringResponse(response1, {
            .res = http::status::ok
        });
        CheckStringResponse(response2, {
            .res = http::status::ok
        });

        gm::State state1, state2;
        INFO(response1.body());
        INFO(response2.body());
        REQUIRE_NOTHROW(state1 = *serializer->DeserializeSessionState(response1.body()));
        REQUIRE_NOTHROW(state2 = *serializer->DeserializeSessionState(response2.body()));
        CHECK(state1 == state2);

        thread1.join();
        thread2.join();
    }
    SECTION("poll_closed"){
        if(MMQueueSuccess(socket, serializer).size() == 1)
            EnqueueNewPlayer(socket, serializer);
        LoginData ld1 = EnqueueNewPlayer(socket, serializer);
        LoginData ld2 = EnqueueNewPlayer(socket, serializer);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token, serializer);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token, serializer));
        
        gm::State state = SessionStateSuccess(socket, serializer, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, serializer, ld2.token, sid));

        std::promise<void> p1, p2, p3;
        std::future<void> f1 = p1.get_future(), f2 = p2.get_future(), f3 = p3.get_future();

        StringResponse response1, response2;

        std::thread th1 ([&]{
            response1 = SessionStateChange(socket, ld1.token, sid);
            p1.set_value();
        });
        std::thread th2 ([&]{
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            response2 = SessionStateChange(socket2, ld1.token, sid);
            p2.set_value();
        });
        f1.wait();
        CheckStringResponse(response1, {
            .body = POLL_CLOSED,
            .res = http::status::conflict
        });
        std::thread th3 ([&]{
            response1 = SessionStateChange(socket, ld2.token, sid);
            p3.set_value();
        });

        dm::Login& now_turn = state.now_turn;
        LoginData& ld = ld1.login == now_turn ? ld1 : ld2;
        gm::Player& player = state.players[0].login == now_turn ? state.players[0] : state.players[1];
        WalkSuccess(socket3, serializer, {player.posX + 1, player.posY}, ld.token, sid);

        f2.wait();
        f3.wait();

        CheckStringResponse(response1,{
            .body = response2.body(),
            .res = http::status::ok
        });

        CheckStringResponse(response2,{
            .res = http::status::ok
        });

        gm::State state2 = *serializer->DeserializeSessionState(response1.body());
        CHECK(state != state2);
        th1.join();
        th2.join();
        th3.join();
    }
    SECTION("wrong_sessionId"){
        if(MMQueueSuccess(socket, serializer).size() == 1)
            EnqueueNewPlayer(socket, serializer);
        LoginData ld1 = EnqueueNewPlayer(socket, serializer);
        LoginData ld2 = EnqueueNewPlayer(socket, serializer);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token, serializer);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token, serializer));
        
        gm::State state = SessionStateSuccess(socket, serializer, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, serializer, ld2.token, sid));

        StringResponse response = SessionStateChange(socket2, ld2.token, "ABOBUSSS");
        CheckStringResponse(response, {
            .body = WRONG_SESSIONID,
            .res = http::status::bad_request
        });
    }
    SECTION("url_parameters_error"){
        if(MMQueueSuccess(socket, serializer).size() == 1)
            EnqueueNewPlayer(socket, serializer);
        LoginData ld1 = EnqueueNewPlayer(socket, serializer);
        LoginData ld2 = EnqueueNewPlayer(socket, serializer);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token, serializer);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token, serializer));
        
        gm::State state = SessionStateSuccess(socket, serializer, ld1.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, serializer, ld2.token, sid));

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
}