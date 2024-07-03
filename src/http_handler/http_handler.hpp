#pragma once
#include "static_handler.hpp"
#include "api_handler.hpp"
#include <iostream>

namespace http_handler {
    using namespace serializer;
    class HttpHandler {
    public:
        HttpHandler(HandlerParameters handler_parameters);

        void operator()(HttpRequest&& request, ResponseSender&& sender);
    private:
        void HandleRequest(HttpRequest&& request, ResponseSender&& sender);
        bool IsApiRequest(const HttpRequest& request);

        StaticHandler static_handler_;
        std::shared_ptr<ApiHandler> api_handler_;
    };

}