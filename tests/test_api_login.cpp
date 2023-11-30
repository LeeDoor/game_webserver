#include "socket_functions.hpp"
#include "api_functions.hpp"
using namespace serializer;

TEST_CASE("server logins players and returns valid token", "[api][login]"){
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string NO_SUCH_USER = serializer->SerializeError("no_such_user", "no user with this login or password");

    SECTION("user registers and logins successfully"){
        hh::RegistrationData rd = RegisterSuccess(socket, serializer);
        LoginData ld = LoginSuccess(socket, rd.login, serializer);
    }
    SECTION("cant login"){
        hh::RegistrationData rd;
        SECTION("with wrong login"){
            RegisterSuccess(socket, serializer);
            rd.login = "incorrect login";
            rd.password = VALID_PASS;
        }
        SECTION("with wrong password"){
            rd.login = RegisterSuccess(socket, serializer).login;
            rd.password = "valid_pass_but_wrong01234";
        }
        http::response<http::string_body> response;
        response = Login(socket, rd.login, rd.password, serializer);
        CheckStringResponse(response, 
            {.body = NO_SUCH_USER, 
             .res = http::status::bad_request });
    }
    

}