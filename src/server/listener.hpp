#pragma once
#include "session.hpp"

namespace http_server {

    namespace net = boost::asio;
    namespace sys = boost::system;
    using tcp = net::ip::tcp;
    namespace beast = boost::beast;
    namespace http = beast::http;
    using namespace std::literals;
    typedef http::request <http::string_body> HttpRequest;



    template <typename RequestHandler>
    class Listener : public std::enable_shared_from_this<Listener<RequestHandler>> {
    public:
        template<typename Handler>
        Listener(net::io_context& ioc, const tcp::endpoint& endpoint, Handler&& request_handler) :
                ioc_(ioc), acceptor_(net::make_strand(ioc)), request_handler_(std::forward<Handler>(request_handler)) {
            acceptor_.open(endpoint.protocol());
            acceptor_.set_option(net::socket_base::reuse_address(true));
            acceptor_.bind(endpoint);
            acceptor_.listen(net::socket_base::max_listen_connections);
        }

        void run(){
            do_accept();
        }
    private:
        void do_accept() {
            acceptor_.async_accept(
                    net::make_strand(ioc_),
                    beast::bind_front_handler(&Listener::on_accept, this->shared_from_this())
            );
        }

        void on_accept(sys::error_code ec, tcp::socket socket) {
            if(ec) return;

            async_run_session(std::move(socket));
            do_accept();
        }

        void async_run_session(tcp::socket&& socket) {
            std::make_shared<Session<RequestHandler>>(std::move(socket), request_handler_)->run();
        }

        net::io_context& ioc_;
        tcp::acceptor acceptor_;
        RequestHandler request_handler_;
    };
}