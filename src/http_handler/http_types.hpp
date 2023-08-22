#pragma once
#include <boost/beast.hpp>

namespace net = boost::asio;
namespace sys = boost::system;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;
typedef http::request <http::string_body> HttpRequest;
typedef http::response<http::string_body> StringResponse;
typedef std::function<void(StringResponse&&)> ResponseSender;
