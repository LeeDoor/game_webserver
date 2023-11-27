#include "api_functions.hpp"
#include "socket_functions.hpp"
#include "nick_generator.hpp"
#include "registration_data.hpp"
#include "json_serializer.hpp"

http::response<http::string_body> Register(tcp::socket& socket, const std::string& login, const std::string& password, ISerializer::Ptr serializer){
    hh::RegistrationData rd{login, password};
    std::string rd_json = serializer->SerializeRegData(rd);
    http::request<http::string_body> req{http::verb::post, REGISTER_API, 11};
    req.body() = rd_json;
    req.prepare_payload();

    return GetResponseToRequest(false, req, socket);
}
std::string RegisterSuccess(tcp::socket& socket, ISerializer::Ptr serializer){
    NickGenerator ng;
    std::string nn = ng.GenerateNick();
    http::response<http::string_body> res = Register(socket, nn, VALID_PASS, serializer);
    CheckStringResponse(res, false, http::status::ok, serializer->SerializeEmpty(), "application/json", {});
    return nn;
}
