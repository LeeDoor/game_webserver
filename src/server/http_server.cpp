#include "http_server.hpp"

#include <boost/asio/dispatch.hpp>
#include <iostream>

namespace http_server {
    void ReportError(beast::error_code ec, const std::string &what) {
        std::cout << "error with code " << ec.value() << "\nwhat: " << what << std::endl;
    }
    void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, std::shared_ptr<serializer::ISerializer> serializer, std::shared_ptr<user_data::Players> players) {
        std::make_shared<Listener>(ioc, endpoint, serializer, players)->Run();
    }
}