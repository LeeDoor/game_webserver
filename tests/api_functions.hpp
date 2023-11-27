#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "user_data.hpp"
#include "json_serializer.hpp"
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace hh = http_handler;
using namespace serializer;

#define REGISTER_API "/api/register"
#define VALID_PASS "01Veterduet2000"

http::response<http::string_body> Register(tcp::socket& socket, const std::string& login, const std::string& password, ISerializer::Ptr serializer);
std::string RegisterSuccess(tcp::socket& socket, ISerializer::Ptr serializer);

http::response<http::string_body> Login(tcp::socket&, std::string& login, std::string& password);
database_manager::UserData LoginSuccess(tcp::socket&);