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
        boost::uuids::uuid uuid = uds_->GenerateUuid(); 
        bool add_line_res = uds_->AddLine({uuid, std::move(rd->login), std::move(rd->password)});
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
        std::optional<database_manager::UserData> ud;
        ud = uds_->GetByLoginPassword(std::move(rd->login), std::move(rd->password));
        if(!ud){
            return responser_.SendNoSuchUser(rns);
        }
        token_manager::Token token = ttu_->GenerateToken();
        ttu_->AddTokenToUuid(token, ud->uuid);
        return responser_.SendToken(rns, token);
    }

    
} // http_handler