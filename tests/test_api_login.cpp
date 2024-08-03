#include "socket_functions.hpp"
#include "api_functions.hpp"


TEST_CASE("server logins players and returns valid token", "[api][login]"){
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    
    std::string NO_SUCH_USER = serializer::SerializeError("no_such_user", "no user with this login or password");

    SECTION("user registers and logins successfully"){
        hh::RegistrationData rd = RegisterSuccess(socket);
        LoginData ld = LoginSuccess(socket, rd.login);
    }
    SECTION("cant login"){
        hh::RegistrationData rd;
        SECTION("with wrong login"){
            RegisterSuccess(socket);
            rd.login = "incorrect login";
            rd.password = VALID_PASS;
        }
        SECTION("with wrong password"){
            rd.login = RegisterSuccess(socket).login;
            rd.password = "valid_pass_but_wrong01234";
        }
        http::response<http::string_body> response;
        response = Login(socket, rd.login, rd.password);
        CheckStringResponse(response, 
            {.body = NO_SUCH_USER, 
             .res = http::status::bad_request });
    }
    

}