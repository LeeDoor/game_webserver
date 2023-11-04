#include <catch2/catch_test_macros.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

tcp::socket GetSocketConnection() {
    net::io_context ioc;

    tcp::resolver resolver(ioc); 
    auto const results = resolver.resolve("127.0.0.1", "9999");
    tcp::socket socket(ioc);
    net::connect(socket, results.begin(), results.end());
    return std::move(socket);
}

http::response<http::string_body> ReadResponse(tcp::socket& socket){
    beast::flat_buffer buffer;

    http::response<http::string_body> res;
    http::read(socket, buffer, res);
    return std::move(res);
}

SCENARIO("server launches and responses to test api", "[launch][test][api]") {
    GIVEN("socket connected to server"){
        tcp::socket socket = GetSocketConnection();

        WHEN("/api/test request sent with get method"){
            http::request<http::string_body> req{http::verb::get, "/api/test", 11};
            http::write(socket, req);

            THEN("request must be correct"){
                http::response<http::string_body> response = ReadResponse(socket);
                REQUIRE(response.result() == http::status::ok);
                REQUIRE(response.body() == "{\"LOL\":\"KEK\",\"SASI\":\"LALKA\"}");

                auto content_length_iter = response.find(http::field::content_length);
                REQUIRE(content_length_iter != response.end());

                int content_length = std::stoi(content_length_iter->value().to_string());
                REQUIRE(content_length == response.body().size());

                auto content_type_iter = response.find(http::field::content_type);
                REQUIRE(content_type_iter != response.end());

                REQUIRE(content_type_iter->value().to_string() == "application/json");
            }
        }
        WHEN("/api/test request sent with head method"){
            http::request<http::string_body> req{http::verb::head, "/api/test", 11};
            http::write(socket, req);

            THEN("request must be correct with empty body"){
                http::response<http::string_body> response = ReadResponse(socket);
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
        }
                
    }
}
