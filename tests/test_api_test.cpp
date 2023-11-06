#include <catch2/catch_test_macros.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "json_serializer.hpp"
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace serializer;

void ConnectSocket(net::io_context& ioc, tcp::socket& socket){
    tcp::resolver resolver(ioc); 
    auto const results = resolver.resolve("127.0.0.1", "9999");
    net::connect(socket, results.begin(), results.end());
}

TEST_CASE("server launches and responses to test api", "[api][test]") {
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);

    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();

    SECTION("/api/test request sent with get method"){
        http::request<http::string_body> req{http::verb::get, "/api/test", 11};
        http::write(socket, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> response;
        http::read(socket, buffer, response);

        CHECK(response.result() == http::status::ok);
        CHECK(response.body() == "{\"LOL\":\"KEK\",\"SASI\":\"LALKA\"}");

        auto content_length_iter = response.find(http::field::content_length);
        if(content_length_iter != response.end())
            FAIL_CHECK("no content_length header");
        else{
            int content_length = std::stoi(content_length_iter->value().to_string());
            CHECK(content_length == response.body().size());
        }

        auto content_type_iter = response.find(http::field::content_type);
        if(content_type_iter != response.end())
            FAIL_CHECK("no content_type header");
        else CHECK(content_type_iter->value().to_string() == "application/json");
        
    }
    SECTION("/api/test request sent with head method"){
        http::request<http::string_body> req{http::verb::head, "/api/test", 11};
        http::write(socket, req);

        beast::flat_buffer buffer;
        http::response_parser<http::string_body> parser;
        http::read_header(socket, buffer, parser);
        http::response<http::string_body> response = parser.get();
        
        REQUIRE(response.result() == http::status::ok);
        REQUIRE(response.body() == "");

        auto content_length_iter = response.find(http::field::content_length);
        REQUIRE(content_length_iter != response.end());

        int content_length = std::stoi(content_length_iter->value().to_string());
        REQUIRE(content_length == std::string("{\"LOL\":\"KEK\",\"SASI\":\"LALKA\"}").size());

        auto content_type_iter = response.find(http::field::content_type);
        REQUIRE(content_type_iter != response.end());

        REQUIRE(content_type_iter->value().to_string() == "application/json");
    }
    SECTION("/api/test request sent with any other wrong method"){
        for(http::verb v : {http::verb::put, http::verb::post}){
            http::request<http::string_body> req{v, "/api/test", 11};
            http::write(socket, req);
            
            beast::flat_buffer buffer;

            http::response<http::string_body> response;
            http::read(socket, buffer, response);
            
            REQUIRE(response.result() == http::status::method_not_allowed);

            REQUIRE(response.body() == serializer->SerializeError("wrong_method", "method not allowed"));

            auto content_length_iter = response.find(http::field::content_length);
            REQUIRE(content_length_iter != response.end());

            int content_length = std::stoi(content_length_iter->value().to_string());
            REQUIRE(content_length == response.body().size());

            auto content_type_iter = response.find(http::field::content_type);
            REQUIRE(content_type_iter != response.end());

            REQUIRE(content_type_iter->value().to_string() == "application/json");

            auto allow_iter = response.find(http::field::allow);
            REQUIRE(allow_iter != response.end());

            std::string allow = allow_iter->value().to_string();
            std::transform(allow.begin(), allow.end(), allow.begin(),
                [](unsigned char c){ return std::tolower(c); });
            REQUIRE((allow == "get, head" || allow == "head, get"));
            
        }
    }
    socket.close(); 
}
