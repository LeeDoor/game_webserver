#pragma once
#include "static_handler.hpp"
#include "api_handler.hpp"

namespace http_handler {
    class HttpHandler {
    public:
        HttpHandler(HandlerParameters handler_parameters);

        void operator()(HttpRequest&& request, SessionFunctions&& session_functions);
    private:
        void HandleRequest(HttpRequest&& request, SessionFunctions&& session_functions);

        enum RequestType{
            Static,
            General,
            Game
        };
        RequestType DeclareRequestType(const HttpRequest& request);

        std::map<RequestType, ApiHandler::Ptr> type_to_handler_;
    };

}