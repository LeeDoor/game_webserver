#pragma once
#include "i_user_data_manager.hpp"
#include <vector>
#include "postgres_connection_pool.hpp"

#define CONNECTION_CAPACITY 80

namespace database_manager{
    class UserDataPostgres : public IUserDataManager {
    public:
        UserDataPostgres();

        bool AddLine(UserData& user_data) override;
        std::optional<UserData> GetByUuid(const std::string& uuid) override;
        std::optional<UserData> GetByLoginPassword(const std::string& login, const std::string& password) override;

    private:
        ConnectionPool pool_;
    };
}