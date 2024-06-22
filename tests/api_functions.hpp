#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "user_data.hpp"
#include "token.hpp"
#include "json_serializer.hpp"
#include "public_user_data.hpp"
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace hh = http_handler;
using namespace serializer;
using namespace token_manager;

#define LOGIN_API       "/api/login"
#define REGISTER_API    "/api/register"
#define PROFILE_API     "/api/profile"
#define PLAYER_TOKENS_API     "/api/debug/player_tokens"

#define VALID_PASS      "01Veterduet2000"

struct LoginData{
    Token token;
    std::string login;
};

http::response<http::string_body> Register(tcp::socket& socket, const std::string& login, const std::string& password, ISerializer::Ptr serializer);
hh::RegistrationData RegisterSuccess(tcp::socket& socket, ISerializer::Ptr serializer);

http::response<http::string_body> Login(tcp::socket&, const std::string& login, const std::string& password, ISerializer::Ptr serializer);
LoginData LoginSuccess(tcp::socket&, const std::string& nn, ISerializer::Ptr serializer);

http::response<http::string_body> Profile(tcp::socket&, const Token& token, ISerializer::Ptr serializer);
hh::PublicUserData ProfileSuccess(tcp::socket&, const Token& token, ISerializer::Ptr serializer);

std::map<std::string, std::string> PlayerTokens(tcp::socket&, ISerializer::Ptr serializer);