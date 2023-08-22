#pragma once
#include "session.hpp"

namespace http_server {

    class Listener : public std::enable_shared_from_this<Listener> {
    public:
        Listener(net::io_context& ioc, const tcp::endpoint& endpoint) :
                ioc_(ioc), acceptor_(net::make_strand(ioc)) {
            acceptor_.open(endpoint.protocol());
            acceptor_.set_option(net::socket_base::reuse_address(true));
            acceptor_.bind(endpoint);
            acceptor_.listen(net::socket_base::max_listen_connections);
        }

        void Run(){
            DoAccept();
        }
    private:
        void DoAccept() {
            acceptor_.async_accept(
                    net::make_strand(ioc_),
                    beast::bind_front_handler(&Listener::OnAccept, this->shared_from_this())
            );
        }

        void OnAccept(sys::error_code ec, tcp::socket socket) {
            if(ec) return;

            AsyncRunSession(std::move(socket));
            DoAccept();
        }

        void AsyncRunSession(tcp::socket&& socket) {
            std::make_shared<Session> (std::move(socket))->Run();
        }

        net::io_context& ioc_;
        tcp::acceptor acceptor_;
    };
}