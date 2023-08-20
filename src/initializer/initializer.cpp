#include "initializer.hpp"
#include "http_server.hpp"
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <iostream>

#define PORT 8888

namespace initializer {
template <typename Fn>
void RunWorkers(unsigned num_threads, const Fn& fn) {
    num_threads = std::max(1u, num_threads);
    std::vector<std::jthread> workers;
    workers.reserve(num_threads - 1);

    while (--num_threads) {
        workers.emplace_back(fn);
    }
    fn();
}


int Initializer::Init(int argc, char **argv) {
    return StartServer();
}


int Initializer::StartServer() {
    namespace net = boost::asio;

    unsigned num_threads = std::thread::hardware_concurrency(); // number of threads

    net::io_context ioc (static_cast<int>(num_threads)); // core io functionality for socket users
    boost::asio::ip::tcp::acceptor acceptor (net::make_strand(ioc), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));

    auto handler = [] (auto&& a, auto&& b){
        std::cout << "bababa" << std::endl;
    };
    const auto address = net::ip::make_address("127.0.0.1");
    constexpr net::ip::port_type port = PORT;
    http_server::serve_http(ioc, {address, port}, [handler](auto &&req, auto &&send) {
        handler(
                std::forward<decltype(req)>(req),
                std::forward<decltype(send)>(send)
        );
    });
    RunWorkers(num_threads, [&ioc]{
        ioc.run();
    });
    return 0;
}

}