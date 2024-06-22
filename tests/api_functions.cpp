#include "api_functions.hpp"
#include "socket_functions.hpp"
#include "nick_generator.hpp"
#include "registration_data.hpp"
#include "json_serializer.hpp"
#include <sstream>

http::response<http::string_body> Register(tcp::socket& socket, const std::string& login, const std::string& password, ISerializer::Ptr serializer){
    hh::RegistrationData rd{login, password};
    std::string rd_json = serializer->SerializeRegData(rd);
    http::request<http::string_body> req{http::verb::post, REGISTER_API, 11};
    req.body() = rd_json;
    req.prepare_payload();

    return GetResponseToRequest(false, req, socket);
}
hh::RegistrationData RegisterSuccess(tcp::socket& socket, ISerializer::Ptr serializer){
    NickGenerator ng;
    std::string nn = ng.GenerateNick();
    http::response<http::string_body> res = Register(socket, nn, VALID_PASS, serializer);
    CheckStringResponse(res, 
        {.body = serializer->SerializeEmpty(), 
        .res = http::status::ok});
    return {.login=nn, .password=VALID_PASS};
}

http::response<http::string_body> Login(tcp::socket&socket, const std::string& login, const std::string& password, ISerializer::Ptr serializer) {
    hh::RegistrationData rd{login, password};
    std::string rd_json = serializer->SerializeRegData(rd);
    http::request<http::string_body> req{http::verb::post, LOGIN_API, 11};
    req.body() = rd_json;
    req.prepare_payload();

    return GetResponseToRequest(false, req, socket);
}
LoginData LoginSuccess(tcp::socket&socket, const std::string& nn, ISerializer::Ptr serializer) {
    http::response<http::string_body> res = Login(socket, nn, VALID_PASS, serializer);
    CheckStringResponse(res, {.res = http::status::ok});
    auto given_data = serializer->DeserializeMap(res.body());
    REQUIRE(given_data);
    REQUIRE(given_data->size() == 1);
    REQUIRE(given_data->contains("token"));
    Token token = given_data->at("token");
    return {token, nn};
}

http::response<http::string_body> Profile(tcp::socket& socket, const Token& token, ISerializer::Ptr serializer){
    http::request<http::string_body> request{http::verb::get, PROFILE_API, 11};;

    std::stringstream ss;
    ss << "Bearer " << token;
    request.set(http::field::authorization, ss.str());

    auto response = GetResponseToRequest(false, request, socket);
    return response;
}
hh::PublicUserData ProfileSuccess(tcp::socket& socket, const Token& token, ISerializer::Ptr serializer) {
    http::response<http::string_body> response = Profile(socket, token, serializer);
    CheckStringResponse(response, 
        {.res = http::status::ok});

    auto is_pud = serializer->DeserializePublicUserData(response.body());
    REQUIRE(is_pud);
    return *is_pud;
}


std::map<std::string, std::string> PlayerTokensSuccess(tcp::socket& socket, ISerializer::Ptr serializer) {
    StringResponse response = PlayerTokens(socket, serializer, "leedoor", "123qwe123");
    CheckStringResponse(response, {.res=http::status::ok});
    auto given_map = serializer->DeserializeMap(response.body());
    REQUIRE(given_map.has_value());
    return *given_map;
}

StringResponse PlayerTokens(tcp::socket& socket, ISerializer::Ptr serializer, const std::string& login, const std::string& password){
    http::request<http::string_body> req{http::verb::get, PLAYER_TOKENS_API, 11};;

    req.body() = serializer->SerializeRegData({login, password});
    req.prepare_payload();

    return GetResponseToRequest(false, req, socket);
}