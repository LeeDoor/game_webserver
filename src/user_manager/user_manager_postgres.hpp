#pragma once
#include "i_user_manager.hpp"
#include <vector>
#include "postgres_connection_pool.hpp"

#define CONNECTION_CAPACITY 80

namespace user_manager{
    class UserManagerPostgres : public IUserManager {
    public:
        UserManagerPostgres(bool is_test, std::string&& bd_credentials);

        bool AddLine(hh::RegistrationData& rd) override;
        std::optional<User> GetByUuid(const Uuid& uuid) override;
        std::optional<User> GetByLoginPassword(const Login& login, const Password& password) override;

    private:
        ConnectionPool pool_;
    };
}