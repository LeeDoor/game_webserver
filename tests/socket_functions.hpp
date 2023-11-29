#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <catch2/catch_test_macros.hpp>
#include "json_serializer.hpp"
#include "../src/http_handler/http_types.hpp"
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace hh = http_handler;
using tcp = net::ip::tcp;

template<typename T>
using opt = std::optional<T>;
using vecstr = std::vector<std::string>;

void ConnectSocket(net::io_context& ioc, tcp::socket& socket);

StringResponse GetResponseToRequest(bool is_head, 
            HttpRequest& request, tcp::socket& socket);

//struct contains data that is required to check
//if data == std::nullopt, this data not being checked
struct ResponseParams{
    //required body
    opt<std::string> body = std::nullopt;
    //is head or not. head check depends on body == std::optional
    bool head             = false;
    //type of response
    opt<std::string> type = "application/json";
    //content_length. if != body != std::optional, len = body.size()
    //overwise len keeps its value
    opt<int> len          = 0;
    //result enum of response
    opt<http::status> res = std::nullopt;
    //allowed methods if response is method_not_allowed
    opt<vecstr> allowed   = std::nullopt;
};

void CheckStringResponse(const StringResponse& response, ResponseParams rp);

