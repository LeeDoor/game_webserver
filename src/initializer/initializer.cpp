#include "initializer.hpp"
#include "http_server.hpp"
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
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

Initializer::Args Initializer::ParseParameters(int argc, char** argv){
    namespace po = boost::program_options;
    using namespace std::literals; 

    po::options_description desc{"All options"s};
    Args args;
    desc.add_options()
        ("test", "test launch to use test bd")
        ("static_path", po::value(&args.static_path)->value_name("dir"), "set path to static library");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    args.test = vm.count("test");
    if(!vm.contains("static_path")){
        throw std::runtime_error("static path is not specified");
    }
    return args;
}

int Initializer::Init(int argc, char **argv) {
    Args args = ParseParameters(argc, argv);
    return StartServer(args);
}

int Initializer::StartServer(Args args) {
    namespace net = boost::asio;

    unsigned num_threads = std::thread::hardware_concurrency(); // number of threads

    net::io_context ioc (static_cast<int>(num_threads)); // core io functionality for socket users

    const auto address = net::ip::make_address("127.0.0.1");
    constexpr net::ip::port_type port = PORT;

    http_handler::HandlerParameters handler_parameters;
    handler_parameters.serializer = std::make_shared<serializer::JSONSerializer>();
    handler_parameters.user_data_manager = std::make_shared<database_manager::UserDataPostgres>(args.test);
    handler_parameters.token_to_uuid = std::make_shared<token_manager::TokenToUuid>();
    handler_parameters.game_manager = std::make_shared<game_manager::GameManager>();
    handler_parameters.mm_queue = 
        std::make_shared<matchmaking_system::MMQueue>(handler_parameters.game_manager);

    std::shared_ptr<std::string> static_path = 
        std::make_shared<std::string>(args.static_path);
    http_server::ServeHttp(ioc, {address, port}, handler_parameters, static_path);
    RunWorkers(num_threads, [&ioc]{
        ioc.run();
    });
    return 0;
}

}