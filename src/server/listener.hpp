#pragma once
#include "session.hpp"

namespace http_server {

    class Listener : public std::enable_shared_from_this<Listener> {
    public:
        Listener(net::io_context& ioc, const tcp::endpoint& endpoint, 
                http_handler::HandlerParameters handler_parameters);

        void Run();
    private:
        void DoAccept();

        void OnAccept(sys::error_code ec, tcp::socket socket);

        void AsyncRunSession(tcp::socket&& socket);

        net::io_context& ioc_;
        tcp::acceptor acceptor_;
        http_handler::HandlerParameters handler_parameters_;

        std::vector<Session::Ptr> sessions_;
    };
}