#pragma once
#include "listener.hpp"

namespace http_server {

void ReportError(beast::error_code ec, const std::string& what);
void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, http_handler::HandlerParameters handler_parameters);

}  // namespace http_server