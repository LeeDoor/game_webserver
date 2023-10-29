#pragma once
#include "listener.hpp"

namespace http_server {

void ReportError(beast::error_code ec, const std::string& what);

void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, std::shared_ptr<serializer::ISerializer> serializer, std::shared_ptr<user_data::Players> players);

}  // namespace http_server