#pragma once
#include "i_user_data_manager.hpp"
#include <vector>
#include "postgres_connection_pool.hpp"

#define CONNECTION_CAPACITY 80

namespace database_manager{
    class UserDataPostgres : public IUserDataManager {
    public:
        UserDataPostgres(bool is_test);

        bool AddLine(hh::RegistrationData& rd) override;
        std::optional<UserData> GetByUuid(const Uuid& uuid) override;
        std::optional<UserData> GetByLoginPassword(const Login& login, const Password& password) override;

    private:
        ConnectionPool pool_;
    };
}