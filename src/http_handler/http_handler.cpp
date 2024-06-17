#include "http_handler.hpp"
#include <iostream>
namespace http_handler{
    HttpHandler::HttpHandler(HandlerParameters handler_parameters) 
        : static_handler_(handler_parameters){
            api_handler_ = std::make_shared<ApiHandler>(handler_parameters);
            api_handler_->Init();
        }

    void HttpHandler::operator()(HttpRequest &&request, ResponseSender&& sender) {
        HandleRequest(std::move(request), std::move(sender));
    }
    void HttpHandler::HandleRequest(HttpRequest&& request, ResponseSender&& sender) {
        if(IsApiRequest(request)) {
            api_handler_->HandleApiFunction(std::move(request), std::move(sender));
        }
        else{
            static_handler_.HandleFile(std::move(request),
                fs::path("/home/leedor/Documents/hex_chess_backend/static"), 
                std::move(sender));
        }
    }

    bool HttpHandler::IsApiRequest(const HttpRequest& request) {
        return request.target().starts_with("/api/");
    }
}