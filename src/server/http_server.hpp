#pragma once
#include "listener.hpp"

namespace http_server {

namespace net = boost::asio;
namespace sys = boost::system;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;
typedef http::request<http::string_body> HttpRequest;

void report_error(beast::error_code ec, const std::string& what);

template <typename RequestHandler>
void serve_http(net::io_context& ioc, const tcp::endpoint& endpoint, RequestHandler&& handler) {
    using MyListener = Listener<std::decay_t<RequestHandler>>;
    std::make_shared<MyListener>(ioc, endpoint, std::forward<RequestHandler>(handler))->run();
}

}  // namespace http_server