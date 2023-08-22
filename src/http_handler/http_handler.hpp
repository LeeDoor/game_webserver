#pragma once
#include "static_handler.hpp"
#include "api_handler.hpp"
#include <iostream>

namespace http_handler {

    class HttpHandler {
    public:
        void operator()(HttpRequest&& request, ResponseSender&& response_func);
    private:
        void HandleRequest(HttpRequest&& request, ResponseSender&& response_func);
        bool IsApiRequest(const HttpRequest& request);

        StaticHandler static_handler_;
        ApiHandler api_handler_;
    };

}