#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <catch2/catch_test_macros.hpp>
#include "json_serializer.hpp"
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace hh = http_handler;
using tcp = net::ip::tcp;

void ConnectSocket(net::io_context& ioc, tcp::socket& socket);

void CheckStringResponse(const http::response<http::string_body>& response, 
                        bool is_head, 
                        http::status status, 
                        std::string&& body, 
                        std::string&& content_type, 
                        std::vector<std::string>&& allow_expected);

http::response<http::string_body> GetResponseToRequest(bool is_head, 
            http::request<http::string_body>& request, tcp::socket& socket);