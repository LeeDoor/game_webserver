#pragma once
#include "../server/beast_typedefs.hpp"
#include <iostream>

namespace http_handler {

    class HttpHandler {
    public:
        void operator()(http_server::HttpRequest&& request, http_server::ResponseSender&& response_func){
            std::cout << "handling!" << std::endl;
        }
    };

}