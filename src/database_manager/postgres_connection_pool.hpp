#pragma once
#include <pqxx/pqxx>
#include <memory>
#include <condition_variable>

namespace database_manager{
    class ConnectionPool {
        using PoolType = ConnectionPool;
        using ConnectionPtr = std::shared_ptr<pqxx::connection>;

    public:
        class ConnectionWrapper {
        public:
            ConnectionWrapper(std::shared_ptr<pqxx::connection>&& conn, PoolType& pool) noexcept
                : conn_{std::move(conn)}
                , pool_{&pool} {
            }

            ConnectionWrapper(const ConnectionWrapper&) = delete;
            ConnectionWrapper& operator=(const ConnectionWrapper&) = delete;

            ConnectionWrapper(ConnectionWrapper&&) = default;
            ConnectionWrapper& operator=(ConnectionWrapper&&) = default;

            pqxx::connection& operator*() const& noexcept {
                return *conn_;
            }
            pqxx::connection& operator*() const&& = delete;

            pqxx::connection* operator->() const& noexcept {
                return conn_.get();
            }

            ~ConnectionWrapper() {
                if (conn_) {
                    pool_->ReturnConnection(std::move(conn_));
                }
            }

        private:
            std::shared_ptr<pqxx::connection> conn_;
            PoolType* pool_;
        };

        ConnectionPool(size_t capacity, bool is_test);
        ConnectionWrapper GetConnection();
    private:
        void ReturnConnection(ConnectionPtr&& conn);
        ConnectionPtr ConnectionFactory(bool is_test);

        std::mutex mutex_;
        std::condition_variable cond_var_;
        std::vector<ConnectionPtr> pool_;
        size_t used_connections_ = 0;
    }; 
}



