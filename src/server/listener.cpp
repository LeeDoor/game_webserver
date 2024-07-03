#include "listener.hpp"

namespace http_server {
    Listener::Listener(net::io_context& ioc, const tcp::endpoint& endpoint, 
            http_handler::HandlerParameters handler_parameters) :
            ioc_(ioc), acceptor_(net::make_strand(ioc)), 
            handler_parameters_(handler_parameters) {
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(net::socket_base::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen(net::socket_base::max_listen_connections);
    }

    void Listener::Run() {
        DoAccept();
    }

    void Listener::DoAccept() {
        acceptor_.async_accept(
                net::make_strand(ioc_),
                beast::bind_front_handler(&Listener::OnAccept, this->shared_from_this())
        );
    }

    void Listener::OnAccept(sys::error_code ec, tcp::socket socket) {
        if(ec) {
            return;
        }

        AsyncRunSession(std::move(socket));
        DoAccept();
    }

    void Listener::AsyncRunSession(tcp::socket&& socket) {
        std::make_shared<Session> (std::move(socket), handler_parameters_)->Run();
    }
}