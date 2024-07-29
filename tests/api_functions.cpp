#include "api_functions.hpp"
#include "nick_generator.hpp"
#include "registration_data.hpp"
#include "json_serializer.hpp"
#include <sstream>

std::string SetUrlParameters(const std::string& url, const std::map<std::string, std::string>& params) {
    if (params.empty())
        return url;
    std::stringstream ss;
    ss << url << '?';
    for (auto it = params.begin(); it != params.end(); ++it) {
        if (it != params.begin())
            ss << '&';
        ss << it->first << '=' << it->second;
    }
    return ss.str();
}
void SetAuthorizationHeader(http::request<http::string_body>& request, const std::string& token){
    std::stringstream ss;
    ss << "Bearer " << token;
    request.set(http::field::authorization, ss.str());
}

http::response<http::string_body> Register(tcp::socket& socket, const dm::Login& login, const dm::Password& password, ISerializer::Ptr serializer){
    hh::RegistrationData rd{login, password};
    std::string rd_json = serializer->Serialize(rd);
    http::request<http::string_body> req{http::verb::post, REGISTER_API, 11};
    req.body() = rd_json;
    req.prepare_payload();

    return GetResponseToRequest(false, req, socket);
}
hh::RegistrationData RegisterSuccess(tcp::socket& socket, ISerializer::Ptr serializer){
    NickGenerator ng;
    dm::Login login = ng.GenerateNick();
    http::response<http::string_body> res = Register(socket, login, VALID_PASS, serializer);
    CheckStringResponse(res, 
        {.body = serializer->SerializeEmpty(), 
        .res = http::status::ok});
    return {.login=login, .password=VALID_PASS};
}

http::response<http::string_body> Login(tcp::socket&socket, const dm::Login& login, const dm::Password& password, ISerializer::Ptr serializer) {
    hh::RegistrationData rd{login, password};
    std::string rd_json = serializer->Serialize(rd);
    http::request<http::string_body> req{http::verb::post, LOGIN_API, 11};
    req.body() = rd_json;
    req.prepare_payload();

    return GetResponseToRequest(false, req, socket);
}
LoginData LoginSuccess(tcp::socket&socket, const dm::Login& login, ISerializer::Ptr serializer) {
    http::response<http::string_body> res = Login(socket, login, VALID_PASS, serializer);
    CheckStringResponse(res, {.res = http::status::ok});
    auto given_data = serializer->DeserializeMap(res.body());
    REQUIRE(given_data);
    REQUIRE(given_data->size() == 1);
    REQUIRE(given_data->contains("token"));
    Token token = given_data->at("token");
    return {token, login};
}

http::response<http::string_body> Profile(tcp::socket& socket, const Token& token){
    http::request<http::string_body> request{http::verb::get, PROFILE_API, 11};

    SetAuthorizationHeader(request, token);
    auto response = GetResponseToRequest(false, request, socket);
    return response;
}
hh::PublicUser ProfileSuccess(tcp::socket& socket, const Token& token, ISerializer::Ptr serializer) {
    http::response<http::string_body> response = Profile(socket, token);
    CheckStringResponse(response, 
        {.res = http::status::ok});

    auto is_pud = serializer->DeserializePublicUser(response.body());
    REQUIRE(is_pud);
    return *is_pud;
}

http::response<http::string_body> Enqueue(tcp::socket& socket, const Token& token){
    http::request<http::string_body> request{http::verb::post, ENQUEUE_API, 11};

    SetAuthorizationHeader(request, token);
    auto response = GetResponseToRequest(false, request, socket);
    return response;
}
bool EnqueueSuccess(tcp::socket& socket, const Token& token, ISerializer::Ptr serializer){
    http::response<http::string_body> response = Enqueue(socket, token);
    CheckStringResponse(response, 
        {.body = serializer->SerializeEmpty(),
        .res = http::status::ok});
    return response.body() == serializer->SerializeEmpty();
}

LoginData EnqueueNewPlayer(tcp::socket& socket, ISerializer::Ptr serializer) {
    hh::RegistrationData rd = RegisterSuccess(socket, serializer);
    LoginData ld = LoginSuccess(socket, rd.login, serializer);
    bool res = EnqueueSuccess(socket, ld.token, serializer);
    CHECK(res == true);
    return ld;
}   

http::response<http::string_body> WaitForOpponent(tcp::socket& socket, const Token& token) {
    http::request<http::string_body> request{http::verb::get, WAIT_FOR_OPPONENT_API, 11};

    SetAuthorizationHeader(request, token);
    auto response = GetResponseToRequest(false, request, socket);
    return response;
}
game_manager::SessionId WaitForOpponentSuccess(tcp::socket& socket, const Token& token, ISerializer::Ptr serializer) {
    INFO("CALLING WaitForOpponentSuccess 1");
    http::response<http::string_body> response = WaitForOpponent(socket, token);
    INFO("GOT WaitForOpponentSuccess 1");
    CheckStringResponse(response, 
        {.res = http::status::ok});
    auto map = serializer->DeserializeMap(response.body());
    INFO(response.body());
    REQUIRE(map);
    REQUIRE(map->size() == 1);
    REQUIRE(map->contains("sessionId"));
    return map->at("sessionId");
}

http::response<http::string_body> SessionState(tcp::socket& socket, const Token& token, const gm::SessionId& sid) {
    std::string target = SetUrlParameters(SESSION_STATE_API, {{"sessionId", sid}});
    http::request<http::string_body> request{http::verb::get, target, 11};

    SetAuthorizationHeader(request, token);
    auto response = GetResponseToRequest(false, request, socket);
    return response;
}
gm::State SessionStateSuccess(tcp::socket& socket, ISerializer::Ptr serializer, const Token& token, const gm::SessionId& sid) {
    http::response<http::string_body> response = SessionState(socket, token, sid);
    CheckStringResponse(response, 
        {.res = http::status::ok});
    std::optional<gm::State> state_opt = serializer->DeserializeSessionState(response.body());
    REQUIRE(state_opt);
    return *state_opt;
}
http::response<http::string_body> SessionStateChange(tcp::socket& socket, const Token& token, const gm::SessionId& sid) {
    std::string target = SetUrlParameters(SESSION_STATE_CHANGE_API, {{"sessionId", sid}});
    http::request<http::string_body> request{http::verb::get, target, 11};

    SetAuthorizationHeader(request, token);
    http::write(socket, request);
    beast::flat_buffer buffer;
    http::response<http::string_body> response;
    http::read(socket, buffer, response);
    return response;
}

http::response<http::string_body> Move(tcp::socket& socket, std::string&& body, const Token& token, const gm::SessionId& sid){
    std::string target = SetUrlParameters(MOVE_API, {{"sessionId", sid}});
    http::request<http::string_body> request{http::verb::post, target, 11};

    SetAuthorizationHeader(request, token);
    request.body() = std::move(body);
    request.prepare_payload();
    auto response = GetResponseToRequest(false, request, socket);
    return response;
}
void MoveSuccess(tcp::socket& socket, std::string&& body, const Token& token, const gm::SessionId& sid){
    http::response<http::string_body> response = Move(socket, std::move(body), token, sid);
    CheckStringResponse(response, 
        {
            .body = "{}",
            .res = http::status::ok
        });
}

StringResponse Walk(tcp::socket& socket, ISerializer::Ptr serializer, const gm::Session::WalkData& wd, const Token& token, const gm::SessionId& sid){
    return Move(socket, serializer->Serialize(wd), token, sid);
}
void WalkSuccess(tcp::socket& socket, ISerializer::Ptr serializer, const gm::Session::WalkData& wd, const Token& token, const gm::SessionId& sid){
    return MoveSuccess(socket, serializer->Serialize(wd), token, sid);
}

std::map<Token, dm::Uuid> PlayerTokensSuccess(tcp::socket& socket, ISerializer::Ptr serializer) {
    StringResponse response = PlayerTokens(socket, serializer, ADMIN_LOGIN, ADMIN_PASSWORD);
    CheckStringResponse(response, {.res=http::status::ok});
    auto given_map = serializer->DeserializeMap(response.body());
    REQUIRE(given_map.has_value());
    return *given_map;
}

StringResponse PlayerTokens(tcp::socket& socket, ISerializer::Ptr serializer, const dm::Login& login, const dm::Password& password){
    http::request<http::string_body> req{http::verb::get, PLAYER_TOKENS_API, 11};

    req.body() = serializer->Serialize(hh::RegistrationData{login, password});
    req.prepare_payload();

    return GetResponseToRequest(false, req, socket);
}

StringResponse User(
    tcp::socket& socket, 
    ISerializer::Ptr serializer, 
    const std::string& Admlogin, 
    const std::string& Admpassword,
    const dm::Login& Usrlogin, 
    const dm::Password& Usrpassword) {

    std::string target = SetUrlParameters(user_API, {{"login", Usrlogin}, {"password", Usrpassword}});
    http::request<http::string_body> req{http::verb::get, target, 11};;
    req.body() = serializer->Serialize(hh::RegistrationData{Admlogin, Admpassword});
    req.prepare_payload();

    return GetResponseToRequest(false, req, socket);
}
StringResponse User(
    tcp::socket& socket, 
    ISerializer::Ptr serializer, 
    const std::string& Admlogin, 
    const std::string& Admpassword,
    const dm::Password& Usruuid) {
    
    std::string target = SetUrlParameters(user_API, {{"uuid", Usruuid}});
    http::request<http::string_body> req{http::verb::get, target, 11};
    req.body() = serializer->Serialize(hh::RegistrationData{Admlogin, Admpassword});
    req.prepare_payload();
    
    return GetResponseToRequest(false, req, socket);
}
dm::User UserSuccess(tcp::socket& socket, ISerializer::Ptr serializer, const dm::Login& Usrlogin, const dm::Password& Usrpassword) {
    StringResponse response = User(socket, serializer, ADMIN_LOGIN, ADMIN_PASSWORD, Usrlogin, Usrpassword);
    CheckStringResponse(response, {.res=http::status::ok});
    auto given_user = serializer->DeserializeUser(response.body());
    REQUIRE(given_user.has_value());
    CHECK(given_user->login == Usrlogin);
    CHECK(given_user->password == Usrpassword);
    return *given_user;
}
dm::User UserSuccess(tcp::socket& socket, ISerializer::Ptr serializer, const dm::Uuid& uuid) {
    StringResponse response = User(socket, serializer, ADMIN_LOGIN, ADMIN_PASSWORD, uuid);
    CheckStringResponse(response, {.res=http::status::ok});
    auto given_user = serializer->DeserializeUser(response.body());
    REQUIRE(given_user.has_value());
    CHECK(given_user->uuid == uuid);
    return *given_user;
}

StringResponse MMQueue(tcp::socket& socket, ISerializer::Ptr serializer, std::string login, std::string password){
    http::request<http::string_body> req{http::verb::get, MM_QUEUE_API, 11};

    req.body() = serializer->Serialize(hh::RegistrationData{login, password});
    req.prepare_payload();

    return GetResponseToRequest(false, req, socket);
}
std::vector<dm::Uuid> MMQueueSuccess(tcp::socket& socket, ISerializer::Ptr serializer){
    StringResponse response = MMQueue(socket, serializer, ADMIN_LOGIN, ADMIN_PASSWORD);
    CheckStringResponse(response, {.res=http::status::ok});
    auto given_vector = serializer->DeserializeUuids(response.body());
    INFO(response.body());
    REQUIRE(given_vector.has_value());
    return *given_vector;
}