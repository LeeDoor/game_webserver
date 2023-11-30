#include "socket_functions.hpp"
#include "api_functions.hpp"
#include <catch2/catch_get_random_seed.hpp>
using namespace serializer;

TEST_CASE("server register players", "[api][register]") {
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);

    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string LOGIN_TAKEN = serializer->SerializeError("login_taken", "login is already taken");
    std::string WRONG_LOGIN_OR_PASSWORD = serializer->SerializeError("wrong_login_or_password", "login size >= 3 password size >= 6 with digit(s)");
    std::string METHOD_NOT_ALLOWED = serializer->SerializeError("wrong_method", "method not allowed");
    
    SECTION("correct json request must add user only once with given login") {
        hh::RegistrationData rd = RegisterSuccess(socket, serializer);
        SECTION("same request must return error"){
            http::response<http::string_body> response;
            response = Register(socket, rd.login, VALID_PASS, serializer);
            CheckStringResponse(response, 
                {.body = LOGIN_TAKEN, 
                .res = http::status::conflict });
        }
    }
    SECTION ("request with incorrect login or password"){
        std::string login, password;
        SECTION ("request with incorrect login"){
            login = "";
            password = VALID_PASS;
        }
        SECTION ("request with too short password with digit"){
            login = "incorrect_password_test_1";
            password = "pswd1";
        }
        SECTION ("request with normal sized password without digits"){
            login = "incorrect_password_test_2";
            password = "normalsized_password_no_digits";
        }
        SECTION ("request with short password without literals"){
            login = "incorrect_password_test_3";
            password = "123";
        }
        http::response<http::string_body> response;
        response = Register(socket, login, password, serializer);
        CheckStringResponse(response, 
            {.body = WRONG_LOGIN_OR_PASSWORD, 
            .res = http::status::bad_request });
    }
    SECTION ("request with incorrect method must fail"){
        hh::RegistrationData rd{"incorrect_method_login", "incorrect_method_password_1"};
        std::string rd_json = serializer->SerializeRegData(rd);

        for(auto verb : {http::verb::get, http::verb::head, http::verb::put, http::verb::move}){
            http::request<http::string_body> req{verb, "/api/register", 11};
            req.body() = rd_json;
            req.prepare_payload();
            http::response<http::string_body> response = GetResponseToRequest(false, req, socket);
            CheckStringResponse(response, 
                {.body = METHOD_NOT_ALLOWED, 
                .res = http::status::method_not_allowed,
                .allowed = vecstr{"post"}});
        }
    }
    socket.close();
}