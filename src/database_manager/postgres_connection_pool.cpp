#include "postgres_connection_pool.hpp"

namespace database_manager{


    ConnectionPool::ConnectionPool(size_t capacity) {
        pool_.reserve(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            pool_.emplace_back(ConnectionFactory());
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

    ConnectionPool::ConnectionPtr ConnectionPool::ConnectionFactory(){
        return std::make_shared<pqxx::connection>("postgres://postgres:1234@localhost:5432/hex_chess");
        //con_ptr->prepare("insert_user", "INSERT INTO users VALUES ($1, $2, $3)");
    }
}