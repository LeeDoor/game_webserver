#include "listener.hpp"
#include "http_server.hpp"

#define CONNECTION_EXPIRY_TIME 30s

namespace http_server {
    void Session::Run() {
        net::dispatch(stream_.get_executor(), beast::bind_front_handler(&Session::Read, GetSharedThis()));
    }

    void Session::Read() {
        request_ = {};
        stream_.expires_after(CONNECTION_EXPIRY_TIME);
        http::async_read(stream_, buffer_, request_,
                         beast::bind_front_handler(&Session::OnRead, GetSharedThis()));
    }

    void Session::OnRead(beast::error_code ec, std::size_t byets_read) {
        if(ec == http::error::end_of_stream) {
            return Close();
        }
        if(ec) {
            return ReportError(ec, "Read");
        }
        HandleRequest(std::move(request_));
    }

    void Session::OnWrite(bool close, beast::error_code ec, std::size_t bytes_written) {
        if(ec) {
            return ReportError(ec, "write");
        }
        if(close) {
            return Close();
        }
        Read();
    }
    void Session::Close() {
        stream_.socket().shutdown(tcp::socket::shutdown_send);
    }
}