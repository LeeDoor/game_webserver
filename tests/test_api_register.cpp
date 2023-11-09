#include "http_functions.hpp"
using namespace serializer;
#include <catch2/catch_get_random_seed.hpp>

TEST_CASE("server launches and register players", "[api][register]") {
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    REQUIRE(socket.is_open());

    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    
    SECTION("correct json request must add user only once with given login") {
        dm::RegistrationData rd{"correct_request_login_", "correct_request_password"};
        int seed = Catch::getSeed();
        rd.login.push_back('a' + seed%26);
        std::string rd_json = serializer->SerializeRegData(rd);
        
        http::request<http::string_body> req{http::verb::post, "/api/register", 11};
        req.body() = rd_json;
        req.prepare_payload();
        http::response<http::string_body> response = GetResponseToRequest(false, req, socket);

        CheckStringResponse(response, false, http::status::ok, serializer->SerializeEmpty(), "application/json", {});
        SECTION("same request must return error"){
            response = GetResponseToRequest(false, req, socket);
            CheckStringResponse(response, false, http::status::conflict, serializer->SerializeError("login_taken", "login is already taken"), "application/json", {});
        }
    }
    SECTION ("request with incorrect login must fail"){
        dm::RegistrationData rd{"", "incorrect_login"};
        std::string rd_json = serializer->SerializeRegData(rd);

        http::request<http::string_body> req{http::verb::post, "/api/register", 11};
        req.body() = rd_json;
        req.prepare_payload();
        http::response<http::string_body> response = GetResponseToRequest(false, req, socket);

        CheckStringResponse(response, false, http::status::bad_request, serializer->SerializeError("wrong_login", "login must have at least 3 symbols"), "application/json", {});
    }
    SECTION ("request with incorrect password must fail"){
        dm::RegistrationData rd{"incorrect_password", ""};
        std::string rd_json = serializer->SerializeRegData(rd);

        http::request<http::string_body> req{http::verb::post, "/api/register", 11};
        req.body() = rd_json;
        req.prepare_payload();
        http::response<http::string_body> response = GetResponseToRequest(false, req, socket);

        CheckStringResponse(response, false, http::status::bad_request, serializer->SerializeError("wrong_password", "password must have at least 6 symbols"), "application/json", {});
    }
    SECTION ("request with incorrect method must fail"){
        dm::RegistrationData rd{"incorrect_method_login", "incorrect_method_password"};
        std::string rd_json = serializer->SerializeRegData(rd);

        for(auto verb : {http::verb::get, http::verb::head, http::verb::put, http::verb::move}){
            http::request<http::string_body> req{verb, "/api/register", 11};
            req.body() = rd_json;
            req.prepare_payload();
            http::response<http::string_body> response = GetResponseToRequest(false, req, socket);
            CheckStringResponse(response, false, http::status::method_not_allowed, serializer->SerializeError("wrong_method", "method not allowed"), "application/json", {"post"});
        }
        
    }

    socket.close();
}