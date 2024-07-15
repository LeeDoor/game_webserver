#include "http_handler.hpp"
#include "general_handler.hpp"
#include "static_handler.hpp"
#include "game_handler.hpp"
#include "debug_handler.hpp"
namespace http_handler{
    HttpHandler::HttpHandler(HandlerParameters handler_parameters) {
        type_to_handler_ = {
            {RequestType::General, std::make_shared<GeneralHandler>(handler_parameters)},
            {RequestType::Game, std::make_shared<GameHandler>(handler_parameters)},
            {RequestType::Static, std::make_shared<StaticHandler>(handler_parameters)},
            {RequestType::Debug, std::make_shared<DebugHandler>(handler_parameters)},
        };
        std::for_each(type_to_handler_.begin(), type_to_handler_.end(), 
            [](std::pair<RequestType, ApiHandler::Ptr> it){it.second->Init();});
    }

    void HttpHandler::operator()(HttpRequest &&request, SessionFunctions&& session_functions) {
        HandleRequest(std::move(request), std::move(session_functions));
    }
    void HttpHandler::HandleRequest(HttpRequest&& request, SessionFunctions&& session_functions) {
        RequestType type = DeclareRequestType(request);
        type_to_handler_[type]->Handle(std::move(request), std::move(session_functions));
    }

    HttpHandler::RequestType HttpHandler::DeclareRequestType(const HttpRequest& request){
        if(request.target().starts_with("/api/")){
            if(request.target().starts_with("/api/game/")){
                return RequestType::Game;
            }
            if(request.target().starts_with("/api/debug/")){
                return RequestType::Debug;
            }
            return RequestType::General;
        }
        return RequestType::Static;
    }
}