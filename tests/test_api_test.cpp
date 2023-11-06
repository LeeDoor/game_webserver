#include <catch2/catch_test_macros.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "json_serializer.hpp"
#include <memory>
#include <boost/algorithm/string/classification.hpp> // for boost::is_any_of
#include <boost/algorithm/string/split.hpp> // boost::split

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

void CheckStringResponse(const http::response<http::string_body>& response, 
                        bool is_head, 
                        http::status status, 
                        std::string&& body, 
                        std::string&& content_type, 
                        std::vector<std::string>&& allow_expected = {}){
    CHECK(response.result() == status);
    if(is_head) {
        CHECK(response.body() == "");
    }
    else {
        CHECK(response.body() == body);
    }

    auto content_length_iter = response.find(http::field::content_length);
    if(content_length_iter == response.end()){
        FAIL_CHECK("no content_length header");
    }
    else{
        int content_length = std::stoi(content_length_iter->value().to_string());
        CHECK(content_length == body.size());
    }

    auto content_type_iter = response.find(http::field::content_type);
    if(content_type_iter == response.end()){
        FAIL_CHECK("no content_type header");
    }
    else {
        CHECK(content_type_iter->value().to_string() == content_type);
    }

    if(status == http::status::method_not_allowed){
        auto allow_iter = response.find(http::field::allow);
        if(allow_iter == response.end()) {
            FAIL_CHECK("no allow header");
        }
        else{
            std::string allow = allow_iter->value().to_string();
            std::transform(allow.begin(), allow.end(), allow.begin(),
                [](unsigned char c){ return std::tolower(c); });
            std::vector<std::string> methods;
            boost::split(methods, allow, boost::is_any_of(", "), boost::token_compress_on);
            std::sort(methods.begin(), methods.end());
            std::sort(allow_expected.begin(), allow_expected.end());
            INFO("|"<<allow<<"|");
            CHECK(allow_expected == methods);
        }
    }
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

        CheckStringResponse(response, false, http::status::ok, "{\"LOL\":\"KEK\",\"SASI\":\"LALKA\"}", "application/json");
    }

    SECTION("/api/test request sent with head method"){
        http::request<http::string_body> req{http::verb::head, "/api/test", 11};
        http::write(socket, req);

        beast::flat_buffer buffer;
        http::response_parser<http::string_body> parser;
        http::read_header(socket, buffer, parser);
        http::response<http::string_body> response = parser.get();
        
        CheckStringResponse(response, true, http::status::ok, "{\"LOL\":\"KEK\",\"SASI\":\"LALKA\"}", "application/json");
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
