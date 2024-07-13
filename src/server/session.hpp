#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include "http_handler.hpp"
#include "i_serializer.hpp"
#include "network_notifier.hpp"

using namespace std::literals;

namespace http_server {
    class Session : public std::enable_shared_from_this<Session> {
    public:
        using Ptr = std::shared_ptr<Session>;

        explicit Session(tcp::socket &&socket, 
            http_handler::HandlerParameters handler_parameters) :
            stream_(std::move(socket)), 
            request_handler_(handler_parameters) {
        }

        Session(const Session &) = delete;

        Session &operator=(const Session &) = delete;

        void Run();

        std::shared_ptr<Session> GetSharedThis();

        void WriteOnce(StringResponse &&response);
    private:
        template<typename ResponseBodyType>
        void Write(http::response<ResponseBodyType> &&response) {
            using resp_ptr = http::response<ResponseBodyType>;
            auto safe_response = std::make_shared<resp_ptr>(std::forward<resp_ptr>(response));
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
        NetworkNotifier::Ptr notifier_;

        std::chrono::seconds expiry_time_ = 30s;
    };

}