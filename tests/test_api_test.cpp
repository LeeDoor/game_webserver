#include "json_serializer.hpp"
#include <memory>
#include "http_functions.hpp"
using namespace serializer;

TEST_CASE("server launches and responses to test api", "[api][test]") {
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    REQUIRE(socket.is_open());

    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();

    SECTION("/api/test request sent with get method"){
        http::request<http::string_body> req{http::verb::get, "/api/test", 11};
        http::write(socket, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(socket, buffer, response);

        CheckStringResponse(response, false, http::status::ok, "{\"LOL\":\"KEK\",\"SASI\":\"LALKA\"}", "application/json", {});
    }

    SECTION("/api/test request sent with head method"){
        http::request<http::string_body> req{http::verb::head, "/api/test", 11};
        http::write(socket, req);

        beast::flat_buffer buffer;
        http::response_parser<http::string_body> parser;
        http::read_header(socket, buffer, parser);
        http::response<http::string_body> response = parser.get();
        
        CheckStringResponse(response, true, http::status::ok, "{\"LOL\":\"KEK\",\"SASI\":\"LALKA\"}", "application/json", {});
    }

    SECTION("/api/test request sent with any other wrong method"){
        for(http::verb v : {http::verb::put, http::verb::post}){
            http::request<http::string_body> req{v, "/api/test", 11};
            http::write(socket, req);
            
            beast::flat_buffer buffer;

            http::response<http::string_body> response;
            http::read(socket, buffer, response);
            
            CheckStringResponse(response, false, http::status::method_not_allowed, serializer->SerializeError("wrong_method", "method not allowed"), "application/json", {"get", "head"});
        }
    }

    socket.close(); 
}
