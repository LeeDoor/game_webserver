//
// Created by leedoor on 22.08.23.
//

#include "api_handler.hpp"
#include <iostream>

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1)
            

namespace http_handler {
    ApiHandler::ApiHandler(std::shared_ptr<ISerializer> serializer) 
        : serializer_(serializer){ }
    void ApiHandler::Init(){
        BuildTargetsMap();
    }

    void ApiHandler::HandleApiFunction(HttpRequest&& request, ResponseSender&& sender){
        std::string function = static_cast<std::string>(request.target());
        if(request_to_executor_.contains(function)) {
            ApiFunctionExecutor& executor = request_to_executor_.at(function);
            ApiStatus result = executor.Execute(request, sender);
            if(result != ApiStatus::Ok)
                return HandleApiError(result, executor, sender.string);
        }
        else{
            SendWrongApiFunction(sender.string);
        }

    }

    void ApiHandler::BuildTargetsMap() {
        ApiFunctionBuilder builder;
        request_to_executor_.emplace(
            std::make_pair("/api/gethellojson/test", 
                builder.GetHead().ExecFunc(BIND(&ApiHandler::ApiGetTestJson)).GetProduct()));
    }

    void ApiHandler::ApiGetTestJson(const ResponseSender& sender) {
        ResponseBuilder<http::string_body> builder;
        std::string body =  serializer_->SerializeMap({{"SASI", "LALKA"}, {"LOL", "KEK"}});
        sender.string(builder.BodyText(std::move(body)).Status(status::ok).GetProduct());
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