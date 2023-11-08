#include "initializer.hpp"
#include "http_server.hpp"
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <iostream>
#include "json_serializer.hpp"
#include "user_data_postgres.hpp"

#define PORT 9999

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

    const auto address = net::ip::make_address("127.0.0.1");
    constexpr net::ip::port_type port = PORT;

    http_handler::HandlerParameters handler_parameters;
    handler_parameters.serializer = std::make_shared<serializer::JSONSerializer>();
    handler_parameters.user_data_manager = std::make_shared<database_manager::UserDataPostgres>();

    http_server::ServeHttp(ioc, {address, port}, handler_parameters);
    RunWorkers(num_threads, [&ioc]{
        ioc.run();
    });
    return 0;
}

}