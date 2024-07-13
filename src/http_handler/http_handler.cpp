#include "http_handler.hpp"
namespace http_handler{
    HttpHandler::HttpHandler(HandlerParameters handler_parameters) {
            api_handler_ = std::make_shared<ApiHandler>(handler_parameters);
            api_handler_->Init();

            static_handler_ = std::make_shared<StaticHandler>(handler_parameters);
        }

    void HttpHandler::operator()(HttpRequest &&request, SessionFunctions&& session_functions) {
        HandleRequest(std::move(request), std::move(session_functions));
    }
    void HttpHandler::HandleRequest(HttpRequest&& request, SessionFunctions&& session_functions) {
        switch (DeclareRequestType(request)){
            case RequestType::Game:
                game_handler_->Handle(std::move(request), std::move(session_functions));
                break;
            case RequestType::Api:
                api_handler_->Handle(std::move(request), std::move(session_functions));
                break;
            case RequestType::Static:
                static_handler_->Handle(std::move(request), std::move(session_functions));
                break;
        }
    }

    HttpHandler::RequestType HttpHandler::DeclareRequestType(const HttpRequest& request){
        if(request.target().starts_with("/api/")){
            if(request.target().starts_with("/api/game/")){
                return RequestType::Game;
            }
            return RequestType::Api;
        }
        return RequestType::Static;
    }
}