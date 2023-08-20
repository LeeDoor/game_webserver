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

        void Run();

    protected:
        ~SessionBase() = default;

        explicit SessionBase(tcp::socket &&socket) : stream_(std::move(socket)) {}

        template<typename Body, typename Fields>
        void Write(http::response<Body, Fields> &&response) {
            auto safe_response = std::make_shared<http::response<Body, Fields>>(std::move(response));
            auto self = GetSharedThis();

            http::async_write(stream_, *safe_response,
                              [safe_response, self](beast::error_code ec, std::size_t bytes_written) {
                                  self->OnWrite(safe_response->need_eof(), ec, bytes_written);
                              });
        }

        beast::tcp_stream stream_;
    private:
        void Read();

        void OnRead(beast::error_code ec, std::size_t byets_read);

        void OnWrite(bool close, beast::error_code ec, std::size_t bytes_written);

        void Close();

        virtual void HandleRequest(HttpRequest &&request) = 0;

        virtual std::shared_ptr<SessionBase> GetSharedThis() = 0;

        beast::flat_buffer buffer_;
        HttpRequest request_;

    };

}