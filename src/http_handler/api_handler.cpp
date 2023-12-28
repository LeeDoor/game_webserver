#include "api_handler.hpp"
#include <iostream>

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1)
            

namespace http_handler {
    ApiHandler::ApiHandler(HandlerParameters handler_parameters) 
        :   serializer_(handler_parameters.serializer), 
            responser_(handler_parameters.serializer), 
            uds_(handler_parameters.user_data_manager),
            ttu_(handler_parameters.token_to_uuid){}
    void ApiHandler::Init(){
        BuildTargetsMap();
    }

    void ApiHandler::HandleApiFunction(HttpRequest&& request, ResponseSender&& sender){
        std::string function = static_cast<std::string>(request.target());
        RequestNSender rns {request, sender};
        if(request_to_executor_.contains(function)) {
            ApiFunctionExecutor& executor = request_to_executor_.at(function);
            ApiStatus result = executor.Execute(rns);
            if(result != ApiStatus::Ok)
                return responser_.HandleApiError(result, executor, rns);
        }
        else{
            responser_.SendWrongApiFunction(rns);
        }

    }

    void ApiHandler::BuildTargetsMap() {
        ApiFunctionBuilder builder;
        request_to_executor_ = {
            { "/api/register", builder.Post().ExecFunc(BIND(&ApiHandler::ApiRegister)).GetProduct() },
            { "/api/login", builder.Post().ExecFunc(BIND(&ApiHandler::ApiLogin)).GetProduct() },
            { "/api/profile", builder.GetHead().ExecFunc(BIND(&ApiHandler::ApiGetProfileData)).GetProduct() },
        };
    }

    void ApiHandler::ApiRegister(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::optional<RegistrationData> rd;
        rd = serializer_->DeserializeRegData(rns.request.body());
        if(!rd.has_value()) {
            return responser_.SendWrongBodyData(rns);
        }
        if(!ValidateRegData(*rd)){
            return responser_.SendWrongLoginOrPassword(rns);
        }
        database_manager::UserData ud {"", std::move(rd->login), std::move(rd->password)};
        bool add_line_res = uds_->AddLine(ud);
        if(!add_line_res){
            return responser_.SendLoginTaken(rns);
        }
        return responser_.SendSuccess(rns);
    }
    void ApiHandler::ApiLogin(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::optional<RegistrationData> rd;
        rd = serializer_->DeserializeRegData(rns.request.body());
        if(!rd.has_value()) {
            return responser_.SendWrongBodyData(rns);
        }
        if(!ValidateRegData(*rd)){
            return responser_.SendWrongLoginOrPassword(rns);
        }
        std::optional<dm::UserData> ud;
        ud = uds_->GetByLoginPassword(std::move(rd->login), std::move(rd->password));
        if(!ud){
            return responser_.SendNoSuchUser(rns);
        }
        tokenm::Token token = ttu_->GenerateToken();
        ttu_->AddTokenToUuid(token, ud->uuid);
        return responser_.SendToken(rns, token);
    }
    void ApiHandler::ApiGetProfileData(RequestNSender rns){
        ResponseBuilder<http::string_body> builder;
        auto auth_iter = rns.request.find(http::field::authorization);
        if (auth_iter == rns.request.end())
            return responser_.SendUnauthorized(rns);

        std::optional<tokenm::Token> token = GetTokenFromHeader(auth_iter->value().to_string());
        if(!token)
            return responser_.SendInvalidToken(rns);

        std::optional<std::string> uuid = ttu_->GetUuidByToken(*token);
        if(!uuid)
            return responser_.SendInvalidToken(rns);

        auto user_data = uds_->GetByUuid(*uuid);
        if(!user_data)
            return responser_.SendTokenToRemovedPerson(rns);
            
        return responser_.SendUserData(rns, *user_data);
    }

    std::optional<tokenm::Token> ApiHandler::GetTokenFromHeader(const std::string& header){
        if(header.substr(0, 7) == "Bearer "){
            tokenm::Token token = header.substr(7);
            if(token.size() == 32)
                return token;
        }
        return std::nullopt;
    }
    
} // http_handler