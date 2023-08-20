#pragma once
#include "session_base.hpp"

namespace http_server {

    namespace net = boost::asio;
    namespace sys = boost::system;
    using tcp = net::ip::tcp;
    namespace beast = boost::beast;
    namespace http = beast::http;
    using namespace std::literals;
    typedef http::request <http::string_body> HttpRequest;

    template<typename RequestHandler>
    class Session : public SessionBase, public std::enable_shared_from_this<Session<RequestHandler>> {
    public:
        template<typename Handler>
        Session(tcp::socket &&socket, Handler &&request_handler)
                : SessionBase(std::move(socket)), request_handler_(std::forward<Handler>(request_handler)) {}

    private:
        void HandleRequest(HttpRequest &&request) override {
            std::string ip = stream_.socket().remote_endpoint().address().to_string();
            //logger::LogRequest(request, ip);

            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            request_handler_(std::move(request), [self = this->shared_from_this(), begin](auto &&response) {
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                //logger::LogResponse(response, std::chrono::duration_cast<std::chrono::microseconds>(end - begin));
                self->Write(std::move(response));
            });
        }

        std::shared_ptr <SessionBase> GetSharedThis() override {
            return this->shared_from_this();
        }

        RequestHandler request_handler_;

    };

}