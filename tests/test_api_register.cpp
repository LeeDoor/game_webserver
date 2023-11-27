#include "socket_functions.hpp"
#include "api_functions.hpp"
#include <catch2/catch_get_random_seed.hpp>
using namespace serializer;

TEST_CASE("server register players", "[api][register]") {
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    REQUIRE(socket.is_open());

    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string LOGIN_TAKEN = serializer->SerializeError("login_taken", "login is already taken");
    std::string WRONG_LOGIN_OR_PASSWORD = serializer->SerializeError("wrong_login_or_password", "login size >= 3 password size >= 6 with digit(s)");
    std::string METHOD_NOT_ALLOWED = serializer->SerializeError("wrong_method", "method not allowed");
    
    SECTION("correct json request must add user only once with given login") {
        std::string login = RegisterSuccess(socket, serializer);
        SECTION("same request must return error"){
            http::response<http::string_body> response;
            response = Register(socket, login, VALID_PASS, serializer);
            CheckStringResponse(response, false, http::status::conflict, std::move(LOGIN_TAKEN), "application/json", {});
        }
    }
    SECTION ("request with incorrect login"){
        http::response<http::string_body> response = Register(socket, "", VALID_PASS, serializer);
        std::string required_error;
        CheckStringResponse(response, false, http::status::bad_request, std::move(WRONG_LOGIN_OR_PASSWORD), "application/json", {});
    }
    SECTION ("request with too short password with digit"){
        http::response<http::string_body> response;
        response = Register(socket, "incorrect_password_test_1", "pswd1", serializer);
        CheckStringResponse(response, false, http::status::bad_request, std::move(WRONG_LOGIN_OR_PASSWORD), "application/json", {});
    }
    SECTION ("request with normal sized password without digits"){
        http::response<http::string_body> response;
        response = Register(socket, "incorrect_password_test_2", "normalsized_password_no_digits", serializer);
        CheckStringResponse(response, false, http::status::bad_request, std::move(WRONG_LOGIN_OR_PASSWORD), "application/json", {});
    }
    SECTION ("request with short password without literals"){
        http::response<http::string_body> response;
        response = Register(socket, "incorrect_password_test_3", "123", serializer);
        CheckStringResponse(response, false, http::status::bad_request, std::move(WRONG_LOGIN_OR_PASSWORD), "application/json", {});
    }
    SECTION ("request with incorrect method must fail"){
        hh::RegistrationData rd{"incorrect_method_login", "incorrect_method_password_1"};
        std::string rd_json = serializer->SerializeRegData(rd);

        for(auto verb : {http::verb::get, http::verb::head, http::verb::put, http::verb::move}){
            http::request<http::string_body> req{verb, "/api/register", 11};
            req.body() = rd_json;
            req.prepare_payload();
            http::response<http::string_body> response = GetResponseToRequest(false, req, socket);
            CheckStringResponse(response, false, http::status::method_not_allowed, std::move(METHOD_NOT_ALLOWED), "application/json", {"post"});
        }
        
    }

    socket.close();
}