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

struct ResponseParams{
    opt<std::string> body = std::nullopt;
    opt<bool> head        = false;
    opt<std::string> type = "application/json";
    bool len              = true;
    opt<http::status> res = std::nullopt;
    opt<vecstr> allowed   = std::nullopt;
};

void CheckStringResponse(const StringResponse& response, ResponseParams rp);

