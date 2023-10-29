
#include "api_handler.hpp"
#include <iostream>

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1)
            

namespace http_handler {
    ApiHandler::ApiHandler(std::shared_ptr<ISerializer> serializer, std::shared_ptr<ud::Players> players) 
        : serializer_(serializer), players_(players){ }
    void ApiHandler::Init(){
        BuildTargetsMap();
    }

    void ApiHandler::HandleApiFunction(HttpRequest&& request, ResponseSender&& sender){
        std::string function = static_cast<std::string>(request.target());
        if(request_to_executor_.contains(function)) {
            ApiFunctionExecutor& executor = request_to_executor_.at(function);
            ApiStatus result = executor.Execute({request, sender});
            if(result != ApiStatus::Ok)
                return HandleApiError(result, executor, sender.string);
        }
        else{
            SendWrongApiFunction(sender.string);
        }

    }

    void ApiHandler::BuildTargetsMap() {
        ApiFunctionBuilder builder;
        request_to_executor_ = {
            { "/api/register", builder.Post().ExecFunc(BIND(&ApiHandler::ApiRegister)).GetProduct() },
            { "/api/test", builder.GetHead().ExecFunc(BIND(&ApiHandler::ApiGetTestJson)).GetProduct() },
        };
    }

    void ApiHandler::ApiGetTestJson(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body =  serializer_->SerializeMap({{"SASI", "LALKA"}, {"LOL", "KEK"}});
        rns.sender.string(builder.BodyText(std::move(body)).Status(status::ok).GetProduct());
    }

    void ApiHandler::ApiRegister(RequestNSender rns) {
        std::string body = rns.request.body();
        std::optional<user_data::RegistrationData> pd = serializer_->DeserializeRegData(std::move(body));
        if(pd.has_value()) {
            bool success = players_->RegisterPlayer(std::move(pd->login), std::move(pd->password));
            if(success){
                return SendSuccess(rns.sender.string);
            }
            return SendLoginTaken(rns.sender.string);
        }
        return SendWrongBodyData(rns.sender.string);
    }


    void ApiHandler::SendSuccess(const StrResponseSender& sender) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeEmpty();
        sender(builder.BodyText(std::move(body)).Status(status::ok).GetProduct());
    }
    
    void ApiHandler::SendWrongBodyData(const StrResponseSender& sender) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("body_data_error", "wrong body data");
        sender(builder.BodyText(std::move(body)).Status(status::bad_request).GetProduct());
    }
    void ApiHandler::SendLoginTaken(const StrResponseSender& sender) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("login_taken", "login is already taken");
        sender(builder.BodyText(std::move(body)).Status(status::conflict).GetProduct());
    }
    
        
    void ApiHandler::HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, const StrResponseSender& sender) {
        switch(status) {
        case ApiStatus::WrongMethod:
            SendWrongMethod(executor, sender);
            break;
        default:
            SendUndefinedError(executor, sender);
            break;
        }
    }
    void ApiHandler::SendWrongApiFunction(const StrResponseSender& sender) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("api_error", "wrong api function");
        sender(builder.BodyText(std::move(body)).Status(status::bad_request).GetProduct());
    }
    void ApiHandler::SendWrongMethod(const ApiFunctionExecutor& executor,const StrResponseSender& sender){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("wrong_method", "method not allowed");
        const std::vector<http::verb>& methods = executor.GetApiFunction().GetAllowedMethods();
        sender(builder.BodyText(std::move(body)).Status(status::method_not_allowed).Allow(methods).GetProduct());
    }
    void ApiHandler::SendUndefinedError(const ApiFunctionExecutor& executor,const StrResponseSender& sender){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("undefined_error", "some weird error happened");
        sender(builder.BodyText(std::move(body)).Status(status::bad_request).GetProduct());
    }
} // http_handler