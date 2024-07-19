#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "json_serializer.hpp"
#include "user_data.hpp"
using namespace serializer;

TEST_CASE("ApiWaitForOpponent", "[api][game][wait_for_opponent]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string POLL_CLOSED = serializer->SerializeError("poll_closed", "new poll connected");
    std::string UNAUTHORIZED = serializer->SerializeError("unathorized", "request must be authorized");
    std::string INVALID_TOKEN = serializer->SerializeError("invalid_token", "request authorization is invalid");

    if(MMQueueSuccess(socket, serializer).size() > 0)
        EnqueueWorthyOpponent(socket, serializer); // to operate with empty queue

    SECTION("success"){
        LoginData ld1 = EnqueueWorthyOpponent(socket, serializer);
        LoginData ld2 = EnqueueWorthyOpponent(socket, serializer);
        //here the session with two enqueued players should create
        game_manager::SessionId session_id1 = WaitForOpponentSuccess(socket, ld1.token, serializer);
        game_manager::SessionId session_id2 = WaitForOpponentSuccess(socket, ld2.token, serializer);
        // aware of blocking thread by WaitForOpponentSuccess if opponent not found yet
        REQUIRE(session_id1 == session_id2);
    }

    SECTION("wrong_token"){
        hh::RegistrationData rd = RegisterSuccess(socket, serializer);
        LoginData ld = LoginSuccess(socket, rd.login, serializer);

        http::request<http::string_body> request{http::verb::post, WAIT_FOR_OPPONENT_API, 11};
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