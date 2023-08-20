#include "listener.hpp"
#include "http_server.hpp"

#define CONNECTION_EXPIRY_TIME 30s

namespace http_server {
    void SessionBase::run() {
        net::dispatch(stream_.get_executor(), beast::bind_front_handler(&SessionBase::read, get_shared_this()));
    }

    void SessionBase::read() {
        request_ = {};
        stream_.expires_after(CONNECTION_EXPIRY_TIME);
        http::async_read(stream_, buffer_, request_,
                         beast::bind_front_handler(&SessionBase::on_read, get_shared_this()));
    }

    void SessionBase::on_read(beast::error_code ec, std::size_t byets_read) {
        if(ec == http::error::end_of_stream) {
            return close();
        }
        if(ec) {
            return report_error(ec, "read");
        }
        handle_request(std::move(request_));
    }

    void SessionBase::on_write(bool close, beast::error_code ec, std::size_t bytes_written) {
        if(ec) {
            return report_error(ec, "write");
        }
        if(close) {
            return close();
        }
        read();
    }
    void SessionBase::close() {
        stream_.socket().shutdown(tcp::socket::shutdown_send);
    }
}