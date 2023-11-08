#include "api_handler.hpp"
#include <iostream>

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1)
            

namespace http_handler {
    ApiHandler::ApiHandler(HandlerParameters handler_parameters) 
        : serializer_(handler_parameters.serializer), uds_(handler_parameters.user_data_manager){ }
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
                return HandleApiError(result, executor, rns);
        }
        else{
            SendWrongApiFunction(rns);
        }

    }

    void ApiHandler::BuildTargetsMap() {
        ApiFunctionBuilder builder;
        request_to_executor_ = {
            { "/api/test", builder.GetHead().ExecFunc(BIND(&ApiHandler::ApiGetTestJson)).GetProduct() },
            { "/api/register", builder.Post().ExecFunc(BIND(&ApiHandler::ApiRegister)).GetProduct() },
        };
    }

    void ApiHandler::ApiGetTestJson(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body =  serializer_->SerializeMap({{"SASI", "LALKA"}, {"LOL", "KEK"}});
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::ok).GetProduct());
    }
    
    void ApiHandler::ApiRegister(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::optional<database_manager::RegistrationData> ud;
        ud = serializer_->DeserializeUserData(rns.request.body());
        if(!ud.has_value()) {
            return SendWrongBodyData(rns);
        }
        boost::uuids::uuid uuid = uds_->GenerateUuid(); 
        bool add_line_res = uds_->AddLine({uuid, std::move(ud->login), std::move(ud->password)});
        if(!add_line_res){
            return SendLoginTaken(rns);
        }
        return SendSuccess(rns);
    }

    void ApiHandler::SendSuccess(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeEmpty();
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::ok).GetProduct());
    }
    
    void ApiHandler::SendWrongBodyData(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("body_data_error", "wrong body data");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
    void ApiHandler::SendLoginTaken(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("login_taken", "login is already taken");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::conflict).GetProduct());
    }
    
        
    void ApiHandler::HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, RequestNSender rns) {
        switch(status) {
        case ApiStatus::WrongMethod:
            SendWrongMethod(executor, rns);
            break;
        default:
            SendUndefinedError(executor, rns);
            break;
        }
    }
    void ApiHandler::SendWrongApiFunction(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("api_error", "wrong api function");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
    void ApiHandler::SendWrongMethod(const ApiFunctionExecutor& executor, RequestNSender rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("wrong_method", "method not allowed");
        const std::vector<http::verb>& methods = executor.GetApiFunction().GetAllowedMethods();
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::method_not_allowed).Allow(methods).GetProduct());
    }
    void ApiHandler::SendUndefinedError(const ApiFunctionExecutor& executor, RequestNSender rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("undefined_error", "some weird error happened");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
} // http_handler