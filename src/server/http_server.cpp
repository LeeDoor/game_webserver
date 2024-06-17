#include "http_server.hpp"

#include <boost/asio/dispatch.hpp>
#include <iostream>

namespace http_server {
    void ReportError(beast::error_code ec, const std::string &what) {
        std::cout << "error with code " << ec.value() << "\nwhat: " << what << std::endl;
    }
    void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, http_handler::HandlerParameters handler_parameters, std::shared_ptr<std::string> static_path) {
        std::make_shared<Listener>(ioc, endpoint, handler_parameters, static_path)->Run();
    }
}