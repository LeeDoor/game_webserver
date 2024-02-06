#include "api_handler.hpp"
#include "get_token_from_header.hpp"
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
        std::cout << function << std::endl;
        if(request.method() == http::verb::options){
            ResponseBuilder<http::string_body> builder;
            http::response<http::string_body> product = 
                builder.Allow({http::verb::get, http::verb::head, http::verb::post})
                .Status(status::ok)
                .GetProduct();
            product.set(http::field::access_control_allow_origin, "*");
            product.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
            rns.sender.string(std::move(product));
            return;
        }
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
            { "/api/profile", builder.NeedAuthor(ttu_).GetHead().ExecFunc(BIND(&ApiHandler::ApiGetProfileData)).GetProduct() },
            { "/api/enqueue", builder.Post().ExecFunc(BIND(&ApiHandler::ApiEnqueue)).GetProduct() },
        };
    }

    void ApiHandler::ApiRegister(RequestNSender rns) {
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
        auto auth_iter = rns.request.find(http::field::authorization);
        std::optional<tokenm::Token> token = GetTokenFromHeader(auth_iter->value().to_string());
        std::optional<std::string> uuid = ttu_->GetUuidByToken(*token);
        auto user_data = uds_->GetByUuid(*uuid);
        if(!user_data)
            return responser_.SendTokenToRemovedPerson(rns);
            
        return responser_.SendUserData(rns, *user_data);
    }
    void ApiHandler::ApiEnqueue(RequestNSender rns){ 
        
    }
    
} // http_handler