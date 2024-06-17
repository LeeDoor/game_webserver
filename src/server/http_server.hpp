#pragma once
#include "listener.hpp"

namespace http_server {

void ReportError(beast::error_code ec, const std::string& what);
void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, http_handler::HandlerParameters handler_parameters, std::shared_ptr<std::string> static_path);

}  // namespace http_server