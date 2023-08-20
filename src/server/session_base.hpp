#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>

namespace http_server {

    namespace net = boost::asio;
    namespace sys = boost::system;
    using tcp = net::ip::tcp;
    namespace beast = boost::beast;
    namespace http = beast::http;
    using namespace std::literals;
    typedef http::request<http::string_body> HttpRequest;

    class SessionBase {
    public:
        SessionBase(const SessionBase &) = delete;

        SessionBase &operator=(const SessionBase &) = delete;

        void run();

    protected:
        ~SessionBase() = default;

        explicit SessionBase(tcp::socket &&socket) : stream_(std::move(socket)) {}

        template<typename Body, typename Fields>
        void write(http::response<Body, Fields> &&response) {
            auto safe_response = std::make_shared<http::response<Body, Fields>>(std::move(response));
            auto self = get_shared_this();

            http::async_write(stream_, *safe_response,
                              [safe_response, self](beast::error_code ec, std::size_t bytes_written) {
                                  self->on_write(safe_response->need_eof(), ec, bytes_written);
                              });
        }

        beast::tcp_stream stream_;
    private:
        void read();

        void on_read(beast::error_code ec, std::size_t byets_read);

        void on_write(bool close, beast::error_code ec, std::size_t bytes_written);

        void close();

        virtual void handle_request(HttpRequest &&request) = 0;

        virtual std::shared_ptr<SessionBase> get_shared_this() = 0;

        beast::flat_buffer buffer_;
        HttpRequest request_;

    };

}