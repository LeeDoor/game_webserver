//
// Created by leedoor on 22.08.23.
//

#include "api_handler.hpp"
#include <iostream>
#include "response_maker.hpp"

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2)
            

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
                return HandleApiError(result, executor, request, sender.string);
        }
        else{
            SendWrongApiFunction(request,sender.string);
        }

    }

    void ApiHandler::BuildTargetsMap() {
        ApiFunctionBuilder builder;
        request_to_executor_.emplace(
            std::make_pair("/api/gethellojson/test", 
                builder.SetGetHeadMethods()
                .SetExecutionFunction(BIND(&ApiHandler::ApiGetTestJson)).GetProduct()));
    }

    void ApiHandler::ApiGetTestJson(const HttpRequest& request, const ResponseSender& sender) {
        sender.string(MakeStringResponse(http::status::ok,
            serializer_->SerializeMap({{"SASI", "LALKA"}, {"LOL", "KEK"}}), request));
    }
        
    void ApiHandler::HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, const HttpRequest& request, const StrResponseSender& sender) {
        switch(status) {
        case ApiStatus::WrongMethod:
            SendWrongMethod(request, sender);
            break;
        default:
            SendUndefinedError(request, sender);
            break;
        }
    }
    void ApiHandler::SendWrongApiFunction(const HttpRequest& request, const StrResponseSender& sender) {
        sender(MakeStringResponse(http::status::bad_request,
            serializer_->SerializeError("api_error", "wrong api function"), request));
    }
    void ApiHandler::SendWrongMethod(const HttpRequest& request, const StrResponseSender& sender){
        sender(MakeStringResponse(http::status::method_not_allowed, 
            serializer_->SerializeError("wrong_method", "method not allowed"), request));
    }
    void ApiHandler::SendUndefinedError(const HttpRequest& request, const StrResponseSender& sender){
        sender(MakeStringResponse( http::status::bad_request, 
            serializer_->SerializeError("undefined_error", "some weird error happened"), request));
    }
} // http_handler