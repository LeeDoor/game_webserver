#include "socket_functions.hpp"
#include "api_functions.hpp"
using namespace serializer;
#include <catch2/catch_get_random_seed.hpp>

TEST_CASE("server logins players and returns valid token", "[api][login]"){
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string NO_SUCH_USER = serializer->SerializeError("no_such_user", "no user with this login or password");

    SECTION("user registers and logins successfully"){
        std::string login = RegisterSuccess(socket, serializer);
        LoginData ld = LoginSuccess(socket, login, serializer);
    }
    SECTION("cant login"){
        std::string login, pass;
        SECTION("with wrong login"){
            RegisterSuccess(socket, serializer);
            login = "incorrect login";
            pass = VALID_PASS;
        }
        SECTION("with wrong password"){
            login = RegisterSuccess(socket, serializer);
            pass = "valid_pass_but_wrong01234";
        }
        http::response<http::string_body> response;
        response = Login(socket, login, pass, serializer);
        CheckStringResponse(response, 
            {.body = NO_SUCH_USER, 
             .res = http::status::bad_request });
    }
    

}