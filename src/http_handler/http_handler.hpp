#pragma once
#include "static_handler.hpp"
#include "api_handler.hpp"
#include <iostream>

namespace http_handler {

    class HttpHandler {
    public:
        void operator()(HttpRequest&& request, 
                        StrResponseSender&& string_sender, 
                        FileResponseSender&& file_sender);
    private:
        void HandleRequest(HttpRequest&& request, 
                        StrResponseSender&& string_sender, 
                        FileResponseSender&& file_sender);
        bool IsApiRequest(const HttpRequest& request);

        StaticHandler static_handler_;
        ApiHandler api_handler_;
    };

}