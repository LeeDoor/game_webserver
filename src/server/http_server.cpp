#include "http_server.hpp"

#include <boost/asio/dispatch.hpp>
#include <iostream>

namespace http_server {
    void ReportError(beast::error_code ec, const std::string &what) {
        std::cout << "error with code " << ec.value() << "\nwhat: " << what << std::endl;
    }
    void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint) {
        std::make_shared<Listener>(ioc, endpoint)->Run();
    }
}