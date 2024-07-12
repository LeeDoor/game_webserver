#pragma once
#include "static_handler.hpp"
#include "api_handler.hpp"
#include "game_handler.hpp"

namespace http_handler {
    using namespace serializer;
    class HttpHandler {
    public:
        HttpHandler(HandlerParameters handler_parameters);

        void operator()(HttpRequest&& request, ResponseSender&& sender);
    private:
        void HandleRequest(HttpRequest&& request, ResponseSender&& sender);

        enum RequestType{
            Static,
            Api,
            Game
        };
        RequestType DeclareRequestType(const HttpRequest& request);

        StaticHandler::Ptr static_handler_;
        ApiHandler::Ptr api_handler_;
        GameHandler::Ptr game_handler_;
    };

}