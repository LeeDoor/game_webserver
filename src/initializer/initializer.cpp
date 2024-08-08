#include "initializer.hpp"
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "user_manager_postgres.hpp"
#include "token_manager_redis.hpp"
#include "queue_manager_redis.hpp"
#include "matchmaking_balancer.hpp"
#include "http_server.hpp"
#include "session_state_notifier.hpp"
#include "session_manager_postgres.hpp"

#define PORT 8080

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
        ("postgres_credentials", po::value(&args.postgres_credentials)->value_name("string"), "set bd postgres login and password like that: \"login:password\"")
        ("redis_credentials", po::value(&args.redis_credentials)->value_name("string"), "set bd redis password");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    args.test = vm.count("test");
    if(!vm.contains("static_path")){
        throw std::runtime_error("static path is not specified");
    }
    if(!vm.contains("postgres_credentials")){
        throw std::runtime_error("postgres_credentials is not specified");
    }
    return args;
}

int Initializer::Init(int argc, char **argv) {
    try {
        auto max_size = 1024*1024*5;
        auto max_files = 3;
        auto new_logger = spdlog::rotating_logger_mt("logs", "logs/logs.txt", max_size, max_files);
        spdlog::set_default_logger(new_logger);    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
    spdlog::flush_every(std::chrono::seconds(1));
    spdlog::flush_on(spdlog::level::info);
    Args args = ParseParameters(argc, argv);
    return StartServer(args);
}

int Initializer::StartServer(Args args) {
    namespace net = boost::asio;

    unsigned num_threads = std::thread::hardware_concurrency(); // number of threads

    net::io_context ioc (static_cast<int>(num_threads)); // core io functionality for socket users

    const auto address = net::ip::make_address("0.0.0.0");
    constexpr net::ip::port_type port = PORT;    

    /// REDIS ///
    sw::redis::ConnectionOptions co;
    co.host = "127.0.0.1";  
    co.password = args.redis_credentials;
    std::shared_ptr<sw::redis::Redis> redis_ptr = std::make_shared<sw::redis::Redis>(co);

    if(redis_ptr->ping() != "PONG")
        spdlog::critical("PING is not PONG smh");

    if(args.test){
        redis_ptr->flushall();
    }

    /// POSTGRES ///
    cp::ConnectionPool::Ptr connection_pool = std::make_shared<cp::ConnectionPool>(args.test, std::move(args.postgres_credentials));

    /// HANDLER ///
    http_handler::HandlerParameters hp;
    hp.user_manager = std::make_shared<um::UserManagerPostgres>(connection_pool);
    hp.token_manager = std::make_shared<token_manager::TokenManagerRedis>("token_to_uuid", redis_ptr); 
    hp.session_manager = std::make_shared<sm::SessionManagerPostgres>(connection_pool);
    hp.game_manager = std::make_shared<gm::GameManager>(hp.user_manager, hp.session_manager);
    hp.queue_manager = std::make_shared<gm::QueueManagerRedis>("matchmaking_queue", "matchmaking_set", redis_ptr);
    hp.static_path = args.static_path;

    hp.queue_manager->OnEnqueueSubscribe
        (std::make_shared<game_manager::MatchmakingBalancer>(hp.queue_manager, hp.game_manager));
    notification_system::SessionStateNotifier::Init(hp.game_manager);

    http_server::ServeHttp(ioc, {address, port}, hp);
    RunWorkers(num_threads, [&ioc]{
        ioc.run();
    });
    return 0;
}

}
