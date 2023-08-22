#include "http_handler.hpp"

namespace http_handler{
    void HttpHandler::operator()(HttpRequest &&request, ResponseSender &&response_func) {
        std::cout << "handling!" << std::endl;
    }
}