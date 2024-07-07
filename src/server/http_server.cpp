#include "http_server.hpp"
#include "spdlog/spdlog.h"
#include <boost/asio/dispatch.hpp>

namespace http_server {
    void ReportError(beast::error_code ec, const std::string &what) {
        spdlog::error("error with code {} what: {}", ec.value(), what);  
    }
    void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, http_handler::HandlerParameters handler_parameters) {
        std::make_shared<Listener>(ioc, endpoint, handler_parameters)->Run();
    }
}