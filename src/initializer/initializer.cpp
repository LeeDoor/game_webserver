#include "initializer.hpp"
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <boost/asio.hpp>
#include <iostream>

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


int Initializer::init(int argc, char **argv) {
    return start_server();
}

void handle() {
    std::cout << "babab";
}

int Initializer::start_server() {
    namespace net = boost::asio;

    unsigned num_threads = std::thread::hardware_concurrency(); // number of threads

    net::io_context ioc (static_cast<int>(num_threads)); // core io functionality for socket users
    boost::asio::ip::tcp::acceptor acceptor (net::make_strand(ioc), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8080));
    acceptor.async_accept([&](boost::system::error_code ec, boost::asio::ip::tcp::socket socket){
        handle();
    });
    RunWorkers(num_threads, [&ioc]{
        ioc.run();
    });
}

}