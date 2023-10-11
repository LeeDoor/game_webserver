#pragma once
#include "static_handler.hpp"
#include "api_handler.hpp"
#include <iostream>

namespace http_handler {
    using namespace serializer;
    class HttpHandler {
    public:
        HttpHandler(std::shared_ptr<ISerializer> serializer);

        void operator()(HttpRequest&& request, 
                        StrResponseSender&& string_sender, 
                        FileResponseSender&& file_sender);
    private:
        void HandleRequest(HttpRequest&& request, 
                        StrResponseSender&& string_sender, 
                        FileResponseSender&& file_sender);
        bool IsApiRequest(const HttpRequest& request);

        std::shared_ptr<ISerializer> serializer_;
        StaticHandler static_handler_;
        ApiHandler api_handler_;
    };

}