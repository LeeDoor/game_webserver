#include "initializer.hpp"
#include "http_server.hpp"
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include "json_serializer.hpp"
#include "user_data_postgres.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

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
        ("static_path", po::value(&args.static_path)->value_name("dir"), "set path to static library")
        ("api_path", po::value(&args.api_path)->value_name("file"), "set path to api functions definitions")
        ("bd_credentials", po::value(&args.bd_credentials)->value_name("string"), "set bd login and password like that: \"login:password\"");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    args.test = vm.count("test");
    if(!vm.contains("static_path")){
        throw std::runtime_error("static path is not specified");
    }
    if(!vm.contains("api_path")){
        throw std::runtime_error("api path is not specified");
    }
    if(!vm.contains("bd_credentials")){
        throw std::runtime_error("bd credentials is not specified");
    }
    return args;
}

int Initializer::Init(int argc, char **argv) {
    try 
    {
        auto new_logger = spdlog::basic_logger_mt("new_default_logger", "logs/new-default-log.txt");
        spdlog::set_default_logger(new_logger);    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
    spdlog::flush_every(std::chrono::seconds(1));
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
    handler_parameters.user_data_manager = std::make_shared<database_manager::UserDataPostgres>(args.test, std::move(args.bd_credentials));
    handler_parameters.token_to_uuid = std::make_shared<token_manager::TokenToUuid>();
    handler_parameters.game_manager = std::make_shared<game_manager::GameManager>();
    handler_parameters.mm_queue = 
        std::make_shared<matchmaking_system::MMQueue>(handler_parameters.game_manager);
    handler_parameters.static_path = args.static_path;
    handler_parameters.api_path = std::move(args.api_path);

    http_server::ServeHttp(ioc, {address, port}, handler_parameters);
    RunWorkers(num_threads, [&ioc]{
        ioc.run();
    });
    return 0;
}

}