#include "http_functions.hpp"
using namespace serializer;
#include <catch2/catch_get_random_seed.hpp>

TEST_CASE("server launches and register players", "[api][register]") {
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    REQUIRE(socket.is_open());

    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    
    SECTION("correct json response must add user") {
        dm::RegistrationData rd{"test_login", "123leon123"};
        int seed = Catch::getSeed();
        rd.login.push_back('a' + seed%26);
        std::string rd_json = serializer->SerializeRegData(rd);
        
        http::request<http::string_body> req{http::verb::post, "/api/register", 11};
        INFO(rd_json);
        req.body() = rd_json;
        req.prepare_payload();
        http::write(socket, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(socket, buffer, response);

        CheckStringResponse(response, false, http::status::ok, serializer->SerializeEmpty(), "application/json", {});
        SECTION("same request must return error"){
            http::write(socket, req);

            beast::flat_buffer buffer;
            http::response<http::string_body> response;
            http::read(socket, buffer, response);

            CheckStringResponse(response, false, http::status::conflict, serializer->SerializeError("login_taken", "login is already taken"), "application/json", {});
        }
    }
}