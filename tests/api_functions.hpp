#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "user.hpp"
#include "token.hpp"
#include "json_serializer.hpp"
#include "public_user.hpp"
#include "session.hpp"
#include "socket_functions.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace hh = http_handler;
using namespace serializer;
using namespace token_manager;
namespace um = user_manager;

#define LOGIN_API           "/api/login"
#define REGISTER_API        "/api/register"
#define PROFILE_API         "/api/profile"

#define ENQUEUE_API             "/api/game/enqueue"
#define WAIT_FOR_OPPONENT_API   "/api/game/wait_for_opponent"
#define SESSION_STATE_API       "/api/game/session_state"
#define SESSION_STATE_CHANGE_API       "/api/game/session_state_change"
#define MOVE_API                "/api/game/move"

#define PLAYER_TOKENS_API   "/api/debug/player_tokens"
#define user_API       "/api/debug/user"
#define MM_QUEUE_API        "/api/debug/matchmaking_queue"

#define VALID_PASS      "01Veterduet2000"
#define INVALID_PASS    "qweqwe"
#define INVALID_LOGIN   "unexistinglogin"

#define ADMIN_LOGIN "leedoor"
#define ADMIN_PASSWORD "123qwe123"

struct LoginData{
    Token token;
    um::Login login;
};

// **Success functions does not require checking for validness of the answer and possibility to call.

std::string SetUrlParameters(const std::string& url, const std::map<std::string, std::string>& parameters);
void SetAuthorizationHeader(http::request<http::string_body>& request, const std::string& token);

http::response<http::string_body> Register(tcp::socket& socket, const um::Login& login, const um::Password& password, ISerializer::Ptr serializer);
hh::RegistrationData RegisterSuccess(tcp::socket& socket, ISerializer::Ptr serializer);

http::response<http::string_body> Login(tcp::socket&, const um::Login& login, const um::Password& password, ISerializer::Ptr serializer);
LoginData LoginSuccess(tcp::socket&, const um::Login& login, ISerializer::Ptr serializer);

http::response<http::string_body> Profile(tcp::socket&, const Token& token);
hh::PublicUser ProfileSuccess(tcp::socket&, const Token& token, ISerializer::Ptr serializer);

 /// GAME METHODS // 

http::response<http::string_body> Enqueue(tcp::socket&, const Token& token);
bool EnqueueSuccess(tcp::socket&, const Token& token, ISerializer::Ptr serializer);

//after calling this function there would be added an opponent to the queue 
// and session will be created for given and enqueued player
LoginData EnqueueNewPlayer(tcp::socket&, ISerializer::Ptr serializer);

http::response<http::string_body> WaitForOpponent(tcp::socket&, const Token& token);
game_manager::SessionId WaitForOpponentSuccess(tcp::socket&, const Token& token, ISerializer::Ptr serializer);

http::response<http::string_body> SessionState(tcp::socket&, const Token& token, const gm::SessionId& sid);
gm::State SessionStateSuccess(tcp::socket&, ISerializer::Ptr serializer, const Token& token, const gm::SessionId& sid);

http::response<http::string_body> SessionStateChange(tcp::socket&, const Token& token, const gm::SessionId& sid);

http::response<http::string_body> Move(tcp::socket&, std::string&& body, const Token& token, const gm::SessionId& sid);
void MoveSuccess(tcp::socket&, std::string&& body, const Token& token, const gm::SessionId& sid);

StringResponse Walk(tcp::socket&, ISerializer::Ptr serializer, const gm::Session::WalkData& wd, const Token& token, const gm::SessionId& sid);
void WalkSuccess(tcp::socket&, ISerializer::Ptr serializer, const gm::Session::WalkData& wd, const Token& token, const gm::SessionId& sid);
/// DEBUG METHODS ///

StringResponse PlayerTokens(tcp::socket&, ISerializer::Ptr serializer, const um::Login& login, const um::Password& password);
std::map<Token, um::Uuid> PlayerTokensSuccess(tcp::socket&, ISerializer::Ptr serializer);

StringResponse User(
    tcp::socket&, 
    ISerializer::Ptr serializer, 
    const std::string& Admlogin, 
    const std::string& Admpassword,
    const um::Login& Usrlogin, 
    const um::Password& Usrpassword);
StringResponse User(
    tcp::socket&, 
    ISerializer::Ptr serializer, 
    const std::string& Admlogin, 
    const std::string& Admpassword,
    const um::Uuid& Usruuid);
um::User UserSuccess(tcp::socket&, ISerializer::Ptr serializer, const um::Login& Usrlogin, const um::Password& Usrpassword);
um::User UserSuccess(tcp::socket&, ISerializer::Ptr serializer, const um::Uuid& uuid);

StringResponse MMQueue(tcp::socket&, ISerializer::Ptr serializer, std::string login, std::string password);
std::vector<um::Uuid> MMQueueSuccess(tcp::socket&, ISerializer::Ptr serializer);