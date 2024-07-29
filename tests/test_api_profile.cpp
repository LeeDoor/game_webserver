#include "socket_functions.hpp"
#include "api_functions.hpp"
using namespace serializer;

TEST_CASE("server correctly returns profile data", "[api][profile]"){
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string UNAUTHORIZED = serializer->SerializeError("unathorized", "request must be authorized");
    std::string INVALID_TOKEN = serializer->SerializeError("invalid_token", "request authorization is invalid");

    SECTION ("logining and trying to get profile data with authorization token"){
        hh::RegistrationData rd = RegisterSuccess(socket, serializer);
        LoginData ld = LoginSuccess(socket, rd.login, serializer);
        http_handler::PublicUser pud = ProfileSuccess(socket, ld.token, serializer);
        CHECK(pud.login == rd.login);
        CHECK(pud.password == rd.password);
    }
    SECTION ("logining and trying to get profile data without token"){
        hh::RegistrationData rd = RegisterSuccess(socket, serializer);
        LoginData ld = LoginSuccess(socket, rd.login, serializer);

        http::request<http::string_body> request{http::verb::get, PROFILE_API, 11};
        auto response = GetResponseToRequest(false, request, socket);

        CheckStringResponse(response, 
            {.body = UNAUTHORIZED,
            .res = http::status::unauthorized});

        request.set(http::field::authorization, "Bearer Ffajefiaeoj84");
        response = GetResponseToRequest(false, request, socket);
        CheckStringResponse(response, 
            {.body = INVALID_TOKEN,
            .res = http::status::unauthorized});
    }
    SECTION ("many users registered, everyone can access only their data"){
        std::vector<hh::RegistrationData> reg_datas(10, RegisterSuccess(socket, serializer));
        std::vector<LoginData> login_datas;
        for(auto rd : reg_datas){
            login_datas.push_back(LoginSuccess(socket, rd.login, serializer));
        }

        for(int i = 0; i < login_datas.size(); ++i){
            hh::PublicUser pud = ProfileSuccess(socket, login_datas[i].token, serializer);
            CHECK(pud.login == reg_datas[i].login);
            CHECK(pud.password == reg_datas[i].password);
        }
    }
}