#include "general_handler.hpp"
#include "api_function_director.hpp"
#include "registration_data.hpp"
#include "send_manager.hpp"
#include "send_manager_tm.hpp"
#include "serializer_http.hpp"
#include "spdlog/spdlog.h"
#include <fstream>

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2) 

#define STREAM_POS(stream_name) std::to_string(static_cast<long long>(stream_name.tellg()))
namespace http_handler {
    GeneralHandler::GeneralHandler(HandlerParameters handler_parameters) 
        :   dm_(handler_parameters.user_manager),
            tm_(handler_parameters.token_manager){}

    void GeneralHandler::Init(){
        ApiFunctionsParse();
    }

    void GeneralHandler::ApiFunctionsParse () {
        ApiFunctionDirector afd(tm_);
        request_to_executor_ = {
            {"/api/register", afd.GetRegister(BIND(&GeneralHandler::ApiRegister))},
            {"/api/login", afd.GetLogin(BIND(&GeneralHandler::ApiLogin))},
            {"/api/profile", afd.GetProfile(BIND(&GeneralHandler::ApiGetProfileData))},
        };
    }

    void GeneralHandler::ApiRegister(SessionData&& rns, const RequestData& rd) {
        std::optional<RegistrationData> red;
        red = serializer::DeserializeRegData(rns.request.body());
        if(!red.has_value()) {
            return SendWrongBodyData(rns);
        }
        if(!ValidateRegData(*red)){
            return SendWrongLoginOrPassword(rns);
        }
        bool add_line_res = dm_->AddLine(red->login, red->password);
        if(!add_line_res){
            return SendLoginTaken(rns);
        }
        return SendSuccess(rns);
    }
    void GeneralHandler::ApiLogin(SessionData&& rns, const RequestData& rd) {
        std::optional<RegistrationData> red;
        red = serializer::DeserializeRegData(rns.request.body());
        if(!red.has_value()) {
            return SendWrongBodyData(rns);
        }
        if(!ValidateRegData(*red)){
            return SendWrongLoginOrPassword(rns);
        }
        std::optional<um::User> ud;
        ud = dm_->GetByLoginPassword(std::move(red->login), std::move(red->password));
        if(!ud){
            return SendNoSuchUser(rns);
        }
        tokenm::Token token = tm_->GenerateToken();
        bool res = tm_->AddTokenToUuid(token, ud->uuid);
        if (!res)
            return SendCantLogin(rns);
        return SendToken(rns, token);
    }
    void GeneralHandler::ApiGetProfileData(SessionData&& rns, const RequestData& rd){
        um::Uuid uuid = rd.uuid;
        auto user = dm_->GetByUuid(uuid);
        if(!user)
            return SendTokenToRemovedPerson(rns);
            
        return SendUser(rns, *user);
    }
    
} // http_handler