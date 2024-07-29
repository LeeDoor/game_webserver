#include "general_handler.hpp"
#include "get_token_from_header.hpp"
#include "api_function_director.hpp"
#include "spdlog/spdlog.h"
#include <fstream>

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1) 

#define STREAM_POS(stream_name) std::to_string(static_cast<long long>(stream_name.tellg()))
namespace http_handler {
    GeneralHandler::GeneralHandler(HandlerParameters handler_parameters) 
        :   ApiHandler(handler_parameters),
            dm_(handler_parameters.user_manager),
            tm_(handler_parameters.token_manager){}

    void GeneralHandler::Init(){
        ApiFunctionsParse();
    }

    void GeneralHandler::ApiFunctionsParse () {
        ApiFunctionDirector afd(serializer_, tm_);
        request_to_executor_ = {
            {"/api/register", afd.GetRegister(BIND(&GeneralHandler::ApiRegister))},
            {"/api/login", afd.GetLogin(BIND(&GeneralHandler::ApiLogin))},
            {"/api/profile", afd.GetProfile(BIND(&GeneralHandler::ApiGetProfileData))},
        };
    }

    void GeneralHandler::ApiRegister(SessionData rns) {
        std::optional<RegistrationData> rd;
        rd = serializer_->DeserializeRegData(rns.request.body());
        if(!rd.has_value()) {
            return responser_.SendWrongBodyData(rns);
        }
        if(!ValidateRegData(*rd)){
            return responser_.SendWrongLoginOrPassword(rns);
        }
        bool add_line_res = dm_->AddLine(*rd);
        if(!add_line_res){
            return responser_.SendLoginTaken(rns);
        }
        return responser_.SendSuccess(rns);
    }
    void GeneralHandler::ApiLogin(SessionData rns) {
        std::optional<RegistrationData> rd;
        rd = serializer_->DeserializeRegData(rns.request.body());
        if(!rd.has_value()) {
            return responser_.SendWrongBodyData(rns);
        }
        if(!ValidateRegData(*rd)){
            return responser_.SendWrongLoginOrPassword(rns);
        }
        std::optional<um::User> ud;
        ud = dm_->GetByLoginPassword(std::move(rd->login), std::move(rd->password));
        if(!ud){
            return responser_.SendNoSuchUser(rns);
        }
        tokenm::Token token = tm_->GenerateToken();
        bool res = tm_->AddTokenToUuid(token, ud->uuid);
        if (!res)
            return responser_.SendCantLogin(rns);
        return responser_.SendToken(rns, token);
    }
    void GeneralHandler::ApiGetProfileData(SessionData rns){
        auto token = SenderAuthentication(rns.request);
        auto uuid = tm_->GetUuidByToken(token);
        auto user = dm_->GetByUuid(*uuid);
        if(!user)
            return responser_.SendTokenToRemovedPerson(rns);
            
        return responser_.SendUser(rns, *user);
    }
    
} // http_handler