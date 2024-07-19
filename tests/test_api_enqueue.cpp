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
    std::string IN_THE_MATCH = serializer->SerializeError("in_the_match", "error happened while enqueuing player (already in the match)");

    SECTION ("success_enqueue"){
        std::vector<dm::Uuid> queue = MMQueueSuccess(socket, serializer);
        hh::RegistrationData rd;
        LoginData ld;
        if (queue.size() == 0){
            rd = RegisterSuccess(socket, serializer);
            ld = LoginSuccess(socket, rd.login, serializer);
            bool res = EnqueueSuccess(socket, ld.token, serializer);
            CHECK(res == true);
        }
        else{
            dm::UserData ud = UserDataSuccess(socket, serializer, queue.back());
            rd = {ud.login, ud.password};
            ld = LoginSuccess(socket, rd.login, serializer);
        }
        for(int i = 0; i < 20; ++i){
            StringResponse response = Enqueue(socket, ld.token);
            CheckStringResponse(response, {
                .body=ENQUEUE_ERROR,
                .res=http::status::ok,
            });
        }
    }
    SECTION("cant_enqueue_if_in_match"){
        std::vector<dm::Uuid> queue = MMQueueSuccess(socket, serializer);
        hh::RegistrationData rd1;
        LoginData ld1;
        if (queue.size() == 0){
            rd1 = RegisterSuccess(socket, serializer);
            bool res = EnqueueSuccess(socket, ld1.token, serializer);
            REQUIRE(res == true);
        }
        else{
            dm::UserData ud = UserDataSuccess(socket, serializer, queue.back());
            rd1.login = ud.login;
            rd1.password = ud.password;
            ld1 = LoginSuccess(socket, rd1.login, serializer);
        }
        LoginData ld2 = EnqueueWorthyOpponent(socket, serializer);

        // they get in the same session

        queue = MMQueueSuccess(socket, serializer);
        CHECK(queue.size() == 0); // from queue to session
        
        StringResponse response = Enqueue(socket, ld1.token);
        CheckStringResponse(response, {
            .body = IN_THE_MATCH,
            .res = http::status::bad_request
        });
        response = Enqueue(socket, ld2.token);
        CheckStringResponse(response, {
            .body = IN_THE_MATCH,
            .res = http::status::bad_request
        });
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