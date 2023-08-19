#include "listener.hpp"
#include "http_server.hpp"

#define CONNECTION_EXPIRY_TIME 30s

namespace http_server {
    void SessionBase::Run() {
        net::dispatch(stream_.get_executor(), beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
    }

    void SessionBase::Read() {
        request_ = {};
        stream_.expires_after(CONNECTION_EXPIRY_TIME);
        http::async_read(stream_, buffer_, request_,
                         beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
    }

    void SessionBase::OnRead(beast::error_code ec, std::size_t byets_read) {
        if(ec == http::error::end_of_stream) {
            return Close();
        }
        if(ec) {
            return ReportError(ec, "Read");
        }
        HandleRequest(std::move(request_));
    }

    void SessionBase::OnWrite(bool close, beast::error_code ec, std::size_t bytes_written) {
        if(ec) {
            return ReportError(ec, "write");
        }
        if(close) {
            return Close();
        }
        Read();
    }
    void SessionBase::Close() {
        stream_.socket().shutdown(tcp::socket::shutdown_send);
    }
}