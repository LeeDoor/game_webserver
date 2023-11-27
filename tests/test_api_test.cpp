#include <memory>
#include "socket_functions.hpp"
using namespace serializer;

TEST_CASE("server launches and responses to test api", "[api][test]") {
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    REQUIRE(socket.is_open());

    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();

    SECTION("/api/test request sent with get method"){
        http::request<http::string_body> req{http::verb::get, "/api/test", 11};
        http::response<http::string_body> response = GetResponseToRequest(false, req, socket);

        CheckStringResponse(response, false, http::status::ok, "{\"LOL\":\"KEK\",\"SASI\":\"LALKA\"}", "application/json", {});
    }

    SECTION("/api/test request sent with head method"){
        http::request<http::string_body> req{http::verb::head, "/api/test", 11};
        http::response<http::string_body> response = GetResponseToRequest(true, req, socket);
        
        CheckStringResponse(response, true, http::status::ok, "{\"LOL\":\"KEK\",\"SASI\":\"LALKA\"}", "application/json", {});
    }

    SECTION("/api/test request sent with any other wrong method"){
        for(http::verb v : {http::verb::put, http::verb::post}){
            http::request<http::string_body> req{v, "/api/test", 11};
            http::response<http::string_body> response = GetResponseToRequest(false, req, socket);
            
            CheckStringResponse(response, false, http::status::method_not_allowed, serializer->SerializeError("wrong_method", "method not allowed"), "application/json", {"get", "head"});
        }
    }

    socket.close(); 
}
