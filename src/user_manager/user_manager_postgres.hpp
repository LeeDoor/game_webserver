#pragma once
#include "i_user_manager.hpp"
#include <vector>
#include "postgres_connection_pool.hpp"

#define CONNECTION_CAPACITY 80

namespace user_manager{
    class UserManagerPostgres : public IUserManager {
    public:
        UserManagerPostgres(cp::ConnectionPool::Ptr pool);

        bool AddLine(const Login& login, const Password& password) override;
        std::optional<User> GetByUuid(const Uuid& uuid) override;
        std::optional<User> GetByLogin(const Login& login) override;
        std::optional<User> GetByLoginPassword(const Login& login, const Password& password) override;

    private:
        cp::ConnectionPool::Ptr pool_;
    };
}