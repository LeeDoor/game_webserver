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

#define ENQUEUE_API         "/api/game/enqueue"
#define WAIT_FOR_OPPONENT_API "/api/game/wait_for_opponent"

#define PLAYER_TOKENS_API   "/api/debug/player_tokens"
#define USER_DATA_API       "/api/debug/user_data"
#define MM_QUEUE_API        "/api/debug/matchmaking_queue"

#define VALID_PASS      "01Veterduet2000"
#define INVALID_PASS    "qweqwe"
#define INVALID_LOGIN   "unexistinglogin"

#define ADMIN_LOGIN "leedoor"
#define ADMIN_PASSWORD "123qwe123"

struct LoginData{
    Token token;
    dm::Login login;
};

// **Success functions does not require checking for validness of the answer and possibility to call.

std::string SetUrlParameters(const std::string& url, const std::map<std::string, std::string>& parameters);
void SetAuthorizationHeader(http::request<http::string_body>& request, const std::string& token);

http::response<http::string_body> Register(tcp::socket& socket, const dm::Login& login, const dm::Password& password, ISerializer::Ptr serializer);
hh::RegistrationData RegisterSuccess(tcp::socket& socket, ISerializer::Ptr serializer);

http::response<http::string_body> Login(tcp::socket&, const dm::Login& login, const dm::Password& password, ISerializer::Ptr serializer);
LoginData LoginSuccess(tcp::socket&, const dm::Login& login, ISerializer::Ptr serializer);

http::response<http::string_body> Profile(tcp::socket&, const Token& token);
hh::PublicUserData ProfileSuccess(tcp::socket&, const Token& token, ISerializer::Ptr serializer);

 /// GAME METHODS // 

http::response<http::string_body> Enqueue(tcp::socket&, const Token& token);
bool EnqueueSuccess(tcp::socket&, const Token& token, ISerializer::Ptr serializer);

//after calling this function there would be added an opponent to the queue 
// and session will be created for given and enqueued player
LoginData EnqueueWorthyOpponent(tcp::socket&, ISerializer::Ptr serializer);

http::response<http::string_body> WaitForOpponent(tcp::socket&, const Token& token);
game_manager::SessionId WaitForOpponentSuccess(tcp::socket&, const Token& token, ISerializer::Ptr serializer);

/// DEBUG METHODS ///

StringResponse PlayerTokens(tcp::socket&, ISerializer::Ptr serializer, const dm::Login& login, const dm::Password& password);
std::map<Token, dm::Uuid> PlayerTokensSuccess(tcp::socket&, ISerializer::Ptr serializer);

StringResponse UserData(
    tcp::socket&, 
    ISerializer::Ptr serializer, 
    const std::string& Admlogin, 
    const std::string& Admpassword,
    const dm::Login& Usrlogin, 
    const dm::Password& Usrpassword);
StringResponse UserData(
    tcp::socket&, 
    ISerializer::Ptr serializer, 
    const std::string& Admlogin, 
    const std::string& Admpassword,
    const dm::Uuid& Usruuid);
dm::UserData UserDataSuccess(tcp::socket&, ISerializer::Ptr serializer, const dm::Login& Usrlogin, const dm::Password& Usrpassword);
dm::UserData UserDataSuccess(tcp::socket&, ISerializer::Ptr serializer, const dm::Uuid& uuid);

StringResponse MMQueue(tcp::socket&, ISerializer::Ptr serializer, std::string login, std::string password);
std::vector<dm::Uuid> MMQueueSuccess(tcp::socket&, ISerializer::Ptr serializer);