#include "socket_functions.hpp"
#include "api_functions.hpp"

#include "user.hpp"


TEST_CASE("ApiWaitForOpponent", "[api][game][wait_for_opponent][long_poll]"){
	net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    
    std::string POLL_CLOSED = serializer::SerializeError("poll_closed", "new poll connected");
    std::string UNAUTHORIZED = serializer::SerializeError("unathorized", "request must be authorized");
    std::string INVALID_TOKEN = serializer::SerializeError("invalid_token", "request authorization is invalid");


    SECTION("success_immediately"){
        if(MMQueueSuccess(socket).size() > 0)
            EnqueueNewPlayer(socket); // to operate with empty queue
        LoginData ld1 = EnqueueNewPlayer(socket);
        LoginData ld2 = EnqueueNewPlayer(socket);
        //here the session with two enqueued players should create
        game_manager::SessionId session_id1 = WaitForOpponentSuccess(socket, ld1.token);
        game_manager::SessionId session_id2 = WaitForOpponentSuccess(socket, ld2.token);
        // aware of blocking thread by WaitForOpponentSuccess if opponent not found yet
        REQUIRE(session_id1 == session_id2);
    }

    SECTION ("success_long_poll"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        LoginData ld1 = EnqueueNewPlayer(socket);
        StringResponse response;
        std::promise<void> promise;
        std::future<void> future = promise.get_future();

        std::thread thread([&]{
            response = WaitForOpponent(socket, ld1.token);
            promise.set_value();
        });

        net::io_context ioc2;
        tcp::socket socket2{ioc2};
        ConnectSocket(ioc2, socket2);

        LoginData ld2 = EnqueueNewPlayer(socket2);
        future.wait();
        gm::SessionId sid = WaitForOpponentSuccess(socket2, ld2.token);
        CheckStringResponse(response,{
            .body = serializer::SerializeMap({{"sessionId", sid}}),
            .res = http::status::ok
        });

        thread.join();
    }

    SECTION ("replacing_polls"){
        if(MMQueueSuccess(socket).size() == 1)
            EnqueueNewPlayer(socket);
        
        std::promise<void> promise1, promise2;
        std::future<void> future1 = promise1.get_future();
        std::future<void> future2 = promise2.get_future();

        net::io_context ioc2;
        tcp::socket socket2{ioc2};
        ConnectSocket(ioc2, socket2);

        LoginData ld1 = EnqueueNewPlayer(socket);
        StringResponse response1, response2;

        // calling first wait_for_opponent as first player
        std::thread thread1([&]{
            http::request<http::string_body> request{http::verb::get, WAIT_FOR_OPPONENT_API, 11};

            SetAuthorizationHeader(request, ld1.token);
            http::write(socket, request);
            beast::flat_buffer buffer;
            http::read(socket, buffer, response1);
            promise1.set_value();
        });

        // calling second wait_for_opponent as same player
        // first call should drop
        std::thread thread2([&]{
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            http::request<http::string_body> request{http::verb::get, WAIT_FOR_OPPONENT_API, 11};

            SetAuthorizationHeader(request, ld1.token);
            http::write(socket2, request);
            beast::flat_buffer buffer;
            http::read(socket2, buffer, response2);
            promise2.set_value();
        });
        
        INFO("A");
        future1.get();
        CheckStringResponse(response1, {
            .body = POLL_CLOSED,
            .res = http::status::conflict
        });

        // socket(1) is now freed
        LoginData ld2 = EnqueueNewPlayer(socket);
        INFO("A");
        future2.get();
        INFO("A");
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld2.token);
        INFO("A");
        CheckStringResponse(response2, {
            .body = serializer::SerializeMap({{"sessionId", sid}}),
            .res = http::status::ok
        });


        thread1.join();
        thread2.join();
    }

    SECTION("wrong_token"){
        hh::RegistrationData rd = RegisterSuccess(socket);
        LoginData ld = LoginSuccess(socket, rd.login);

        http::request<http::string_body> request{http::verb::get, WAIT_FOR_OPPONENT_API, 11};
        auto response = GetResponseToRequest(false, request, socket);

        CheckStringResponse(response, 
            {.body = UNAUTHORIZED,
            .res = http::status::unauthorized});

        request.set(http::field::authorization, "Bearer Ffajefiaeoj84");
        response = GetResponseToRequest(false, request, socket);
        CheckStringResponse(response, 
            {.body = INVALID_TOKEN,
            .res = http::status::unauthorized});
    }
}