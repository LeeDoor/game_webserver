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
namespace dm = database_manager;

#define LOGIN_API           "/api/login"
#define REGISTER_API        "/api/register"
#define PROFILE_API         "/api/profile"
#define PLAYER_TOKENS_API   "/api/debug/player_tokens"
#define USER_DATA_API       "/api/debug/user_data"

#define VALID_PASS      "01Veterduet2000"

#define ADMIN_LOGIN "leedoor"
#define ADMIN_PASSWORD "123qwe123"

struct LoginData{
    Token token;
    std::string login;
};

// **Success functions does not require checking for validness of the answer and possibility to call.

std::string SetUrlParameters(const std::string& url, const std::map<std::string, std::string>& parameters);

http::response<http::string_body> Register(tcp::socket& socket, const std::string& login, const std::string& password, ISerializer::Ptr serializer);
hh::RegistrationData RegisterSuccess(tcp::socket& socket, ISerializer::Ptr serializer);

http::response<http::string_body> Login(tcp::socket&, const std::string& login, const std::string& password, ISerializer::Ptr serializer);
LoginData LoginSuccess(tcp::socket&, const std::string& nn, ISerializer::Ptr serializer);

http::response<http::string_body> Profile(tcp::socket&, const Token& token, ISerializer::Ptr serializer);
hh::PublicUserData ProfileSuccess(tcp::socket&, const Token& token, ISerializer::Ptr serializer);

StringResponse PlayerTokens(tcp::socket&, ISerializer::Ptr serializer, const std::string& login, const std::string& password);
std::map<std::string, std::string> PlayerTokensSuccess(tcp::socket&, ISerializer::Ptr serializer);

StringResponse UserData(
    tcp::socket&, 
    ISerializer::Ptr serializer, 
    const std::string& Admlogin, 
    const std::string& Admpassword,
    const std::string& Usrlogin, 
    const std::string& Usrpassword);
StringResponse UserData(
    tcp::socket&, 
    ISerializer::Ptr serializer, 
    const std::string& Admlogin, 
    const std::string& Admpassword,
    const std::string& Usruuid);
dm::UserData UserDataSuccess(tcp::socket&, ISerializer::Ptr serializer, const std::string& Usrlogin, const std::string& Usrpassword);
dm::UserData UserDataSuccess(tcp::socket&, ISerializer::Ptr serializer, const std::string& uuid);
