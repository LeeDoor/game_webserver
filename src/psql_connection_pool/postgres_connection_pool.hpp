#pragma once
#include <pqxx/pqxx>
#include <memory>
#include <condition_variable>

namespace connection_pool{
    class ConnectionPool {
        using PoolType = ConnectionPool;
        using ConnectionPtr = std::shared_ptr<pqxx::connection>;

    public:
        using Ptr = std::shared_ptr<ConnectionPool>;
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

        ConnectionPool(std::string&& bd_credentials);
        ConnectionWrapper GetConnection();
    private:
        void ReturnConnection(ConnectionPtr&& conn);
        ConnectionPtr ConnectionFactory(const std::string& bd_credentials);

        std::mutex mutex_;
        std::condition_variable cond_var_;
        std::vector<ConnectionPtr> pool_;
        size_t used_connections_ = 0;
    }; 
}

namespace cp = connection_pool;