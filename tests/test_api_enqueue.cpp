#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "json_serializer.hpp"
#include "user_data.hpp"
using namespace serializer;

TEST_CASE("ApiEnqueue", "[api][enqueue]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string UNAUTHORIZED = serializer->SerializeError("unathorized", "request must be authorized");
    std::string ENQUEUE_ERROR = serializer->SerializeError("enqueue_error", "error happened while enqueuing player (already in queue)");
    std::string INVALID_TOKEN = serializer->SerializeError("invalid_token", "request authorization is invalid");

    SECTION ("success_enqueue"){
        hh::RegistrationData rd = RegisterSuccess(socket, serializer);
        LoginData ld = LoginSuccess(socket, rd.login, serializer);
        bool res = EnqueueSuccess(socket, ld.token, serializer);
        CHECK(res == true);
        for(int i = 0; i < 20; ++i){
            StringResponse response = Enqueue(socket, ld.token);
            CheckStringResponse(response, {
                .body=ENQUEUE_ERROR,
                .res=http::status::ok,
            });
        }
    }
    SECTION("wrong_token"){
        hh::RegistrationData rd = RegisterSuccess(socket, serializer);
        LoginData ld = LoginSuccess(socket, rd.login, serializer);

        http::request<http::string_body> request{http::verb::post, ENQUEUE_API, 11};
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