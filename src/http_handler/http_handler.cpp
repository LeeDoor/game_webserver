#include "http_handler.hpp"

namespace http_handler{
    void HttpHandler::operator()(HttpRequest &&request,
                                StrResponseSender&& string_sender, 
                                FileResponseSender&& file_sender) {
        HandleRequest(std::move(request), std::move(string_sender), std::move(file_sender));
    }
    void HttpHandler::HandleRequest(HttpRequest&& request, 
                    StrResponseSender&& string_sender, 
                    FileResponseSender&& file_sender) {
        static_handler_.HandleFile(fs::path("/home/leedoor/Documents/hex_chess_backend/static/index.html"),
        fs::path("/home/leedoor/Documents/hex_chess_backend/static"), std::move(string_sender), std::move(file_sender));
    }
}