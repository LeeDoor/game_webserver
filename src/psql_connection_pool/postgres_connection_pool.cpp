#include "postgres_connection_pool.hpp"
#include "spdlog/spdlog.h"
#define MAX_CONNECTION_COUNT 80

namespace connection_pool{
    ConnectionPool::ConnectionPool(std::string&& bd_credentials, std::string&& bd_address) {
        pool_.reserve(MAX_CONNECTION_COUNT);
        for (size_t i = 0; i < MAX_CONNECTION_COUNT; ++i) {
            pool_.emplace_back(ConnectionFactory(bd_credentials, bd_address));
        } 
    }

    ConnectionPool::ConnectionWrapper ConnectionPool::GetConnection() {
        std::unique_lock lock{mutex_};
        cond_var_.wait(lock, [this] {
            return used_connections_ < pool_.size();
        });
        return {std::move(pool_[used_connections_++]), *this};
    }

    void ConnectionPool::ReturnConnection(ConnectionPtr&& conn) {
        {
            std::lock_guard lock{mutex_};
            assert(used_connections_ != 0);
            pool_[--used_connections_] = std::move(conn);
        }
        cond_var_.notify_one();
    }

    ConnectionPool::ConnectionPtr ConnectionPool::ConnectionFactory(const std::string& bd_credentials, const std::string& bd_address){
        std::stringstream ss;
        ss << "postgres://" << bd_credentials << "@" << bd_address << ":5432/game_webserver";
        return std::make_shared<pqxx::connection>(ss.str());
    }
}
