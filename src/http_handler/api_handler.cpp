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
            //pass executor here?
            //TODO: make custom response methods and Allow header for method_not_allowed response
            //make && refs where response is necessary and const& where is optional
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
        sender.string(MakeStringResponse(request, 
            serializer_->SerializeMap({{"SASI", "LALKA"}, {"LOL", "KEK"}})));
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
        sender( MakeStringResponse(request, 
                serializer_->SerializeError("api_error", "wrong api function")));
    }
    void ApiHandler::SendWrongMethod(const HttpRequest& request, const StrResponseSender& sender){
        sender(MakeStringResponse(request, 
            serializer_->SerializeError("wrong_method", "method not allowed")));
    }
    void ApiHandler::SendUndefinedError(const HttpRequest& request, const StrResponseSender& sender){
        sender(MakeStringResponse(request, 
            serializer_->SerializeError("undefined_error", "some weird error happened")));
    }
} // http_handler