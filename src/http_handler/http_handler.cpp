#include "http_handler.hpp"

namespace http_handler{
    void HttpHandler::operator()(http_server::HttpRequest &&request, http_server::ResponseSender &&response_func) {
        std::cout << "handling!" << std::endl;
    }
}