#pragma once
#include <boost/algorithm/string/classification.hpp> // for boost::is_any_of
#include <boost/algorithm/string/split.hpp> // boost::split
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <catch2/catch_test_macros.hpp>
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void ConnectSocket(net::io_context& ioc, tcp::socket& socket);

void CheckStringResponse(const http::response<http::string_body>& response, 
                        bool is_head, 
                        http::status status, 
                        std::string&& body, 
                        std::string&& content_type, 
                        std::vector<std::string>&& allow_expected);