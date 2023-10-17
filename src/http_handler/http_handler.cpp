#include "http_handler.hpp"

namespace http_handler{
    HttpHandler::HttpHandler(std::shared_ptr<ISerializer> serializer) 
        : serializer_(serializer), static_handler_(serializer){
            api_handler_ = std::make_shared<ApiHandler>(serializer);
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
                fs::path("/home/leedoor/Documents/hex_chess_backend/static"), 
                std::move(sender));
        }
    }

    bool HttpHandler::IsApiRequest(const HttpRequest& request) {
        return request.target().starts_with("/api/");
    }
}