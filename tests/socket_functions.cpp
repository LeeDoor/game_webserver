#include "socket_functions.hpp"
#include <boost/algorithm/string/classification.hpp> // for boost::is_any_of
#include <boost/algorithm/string/split.hpp> // boost::split

void ConnectSocket(net::io_context& ioc, tcp::socket& socket){
    tcp::resolver resolver(ioc); 
    auto const results = resolver.resolve("127.0.0.1", "8080");
    net::connect(socket, results.begin(), results.end());
    REQUIRE(socket.is_open());
}

void CheckBody(const StringResponse& given, std::string& body, bool is_head){
    if(is_head) {
        CHECK(given.body() == "");
    }
    else {
        CHECK(given.body() == body);
    }
}
void CheckContentLength(const StringResponse& given, int content_length) {
    auto content_length_iter = given.find(http::field::content_length);
    if(content_length_iter == given.end()){
        FAIL_CHECK("no content_length header");
        return;
    }
    int given_content_length = std::stoi(static_cast<std::string>(content_length_iter->value()));
    CHECK(given_content_length == content_length);
}
void CheckContentType(const StringResponse& given, std::string content_type) {
    auto content_type_iter = given.find(http::field::content_type);
    if(content_type_iter == given.end()){
        FAIL_CHECK("no content_type header");
    }
    else {
        std::string given_content_type = static_cast<std::string>(content_type_iter->value());
        CHECK(given_content_type == content_type);
    }
}
void CheckAllowed(const StringResponse& given, vecstr allow_expected){
    if(given.result() == http::status::method_not_allowed){
        auto allow_iter = given.find(http::field::allow);
        if(allow_iter == given.end()) {
            FAIL_CHECK("no allow header");
        }
        else{
            std::string allow = static_cast<std::string>(allow_iter->value());
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
void CheckStatus(const StringResponse& given, http::status result){
    CHECK(given.result() == result);
}

void CheckStringResponse(const StringResponse& response, ResponseParams rp){
    if(rp.body)
        CheckBody(response, *rp.body, rp.head);
    if(rp.body)
        rp.len = rp.body->size();
    else
        rp.len = response.body().size();
    CheckContentLength(response, rp.len);
    
    CheckContentType(response, rp.type);
    if(rp.res)
        CheckStatus(response, *rp.res);
    if(rp.allowed)
        CheckAllowed(response, *rp.allowed);
}

StringResponse GetResponseToRequest(bool is_head, HttpRequest& request, tcp::socket& socket) {
    REQUIRE_NOTHROW(http::write(socket, request));
    beast::flat_buffer buffer;
    http::response<http::string_body> response;
    if(!is_head){
        REQUIRE_NOTHROW(http::read(socket, buffer, response));
        return response;
    }
    http::response_parser<http::string_body> parser;
    REQUIRE_NOTHROW(http::read_header(socket, buffer, parser));
    response = parser.get();
    return response;
}