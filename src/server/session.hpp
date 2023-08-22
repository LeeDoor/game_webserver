#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include "http_handler.hpp"
#include <memory>

namespace http_server {
    class Session : public std::enable_shared_from_this<Session> {
    public:
        explicit Session(tcp::socket &&socket) : stream_(std::move(socket)) {}

        Session(const Session &) = delete;

        Session &operator=(const Session &) = delete;

        void Run();

        std::shared_ptr<Session> GetSharedThis();

    private:
        template<typename ResponseBodyType>
        void Write(http::response<ResponseBodyType> &&response) {
            auto safe_response = std::make_shared<http::response<ResponseBodyType>>(std::forward<http::response<ResponseBodyType>>(response));
            auto self = GetSharedThis();

            http::async_write(stream_, *safe_response,
                              [safe_response, self](beast::error_code ec, std::size_t bytes_written) {
                                  self->OnWrite(safe_response->need_eof(), ec, bytes_written);
                              });
        }

        void HandleRequest(HttpRequest &&request);

        void Read();

        void OnRead(beast::error_code ec, std::size_t bytes_read);

        void OnWrite(bool close, beast::error_code ec, std::size_t bytes_written);

        void Close();

        beast::flat_buffer buffer_;
        HttpRequest request_;
        beast::tcp_stream stream_;


        http_handler::HttpHandler request_handler_;
    };

}