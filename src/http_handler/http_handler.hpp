#pragma once
#include "static_handler.hpp"
#include "general_handler.hpp"
#include "game_handler.hpp"

namespace http_handler {
    class HttpHandler {
    public:
        HttpHandler(HandlerParameters handler_parameters);

        void operator()(HttpRequest&& request, SessionFunctions&& session_functions);
    private:
        void HandleRequest(HttpRequest&& request, SessionFunctions&& session_functions);

        enum RequestType{
            Static,
            Api,
            Game
        };
        RequestType DeclareRequestType(const HttpRequest& request);

        StaticHandler::Ptr static_handler_;
        ApiHandler::Ptr general_handler_;
        GameHandler::Ptr game_handler_;
    };

}