//
// Created by leedoor on 22.08.23.
//

#include "api_handler.hpp"
#include <iostream>
#include "response_maker.hpp"

namespace http_handler {
    ApiHandler::ApiHandler(std::shared_ptr<ISerializer> serializer) 
        : serializer_(serializer){ }
    void ApiHandler::Init(){
        BuildTargetsMap();
    }

    void ApiHandler::HandleApiFunction(HttpRequest&& request, ResponseSender&& sender){
        std::string function = static_cast<std::string>(request.target());
        if(request_to_executor_.contains(function)) {
            request_to_executor_.at(function).Execute(std::move(request), std::move(sender));
        }
        else{
            SendWrongApiFunction(std::move(request), std::move(sender));
        }

    }

    void ApiHandler::BuildTargetsMap() {
        ApiFunctionBuilder builder;
        request_to_executor_.insert(std::make_pair("/api/gethellojson/test", builder
        .SetGetHeadMethods()
        .SetExecutionFunction(
            (ExecutorFunction)
            std::bind(
                &ApiHandler::ApiGetTestJson, 
                this->shared_from_this(), 
                std::placeholders::_1, 
                std::placeholders::_2)
            ).GetProduct()));
    }

    void ApiHandler::ApiGetTestJson(HttpRequest&& request, ResponseSender&& sender) {
        sender.string(MakeStringResponse(std::move(request), 
        serializer_->SerializeMap({{"SASI", "LALKA"}, {"LOL", "KEK"}})));
    }
        
    void ApiHandler::SendWrongApiFunction(HttpRequest&& request, ResponseSender&& sender) {
        sender.string( MakeStringResponse(std::move(request), 
                serializer_->SerializeError("api_error", "wrong api function")));
    }
} // http_handler