#include "postgres_connection_pool.hpp"

namespace user_manager{


    ConnectionPool::ConnectionPool(size_t capacity, bool is_test, std::string&& bd_credentials) {
        pool_.reserve(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            pool_.emplace_back(ConnectionFactory(bd_credentials));
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

    ConnectionPool::ConnectionPtr ConnectionPool::ConnectionFactory(const std::string& bd_credentials){
        std::stringstream ss;
        ss << "postgres://" << bd_credentials << "@localhost:5432/hex_chess";
        return std::make_shared<pqxx::connection>(ss.str());
    }
}