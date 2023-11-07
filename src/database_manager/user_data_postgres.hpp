#pragma once
#include "i_user_data_manager.hpp"
#include <vector>

namespace database_manager{
    class UserDataPostgres : public IUserDataManager {
    public:
        ids::uuid GenerateUuid() override;
        bool AddLine(UserData&& user_data) override;
        std::optional<UserData> GetByUuid(ids::uuid&& uuid) override;
        std::optional<UserData> GetByLoginPassword(std::string&& login, std::string&& password) override;

    private:
        std::vector<UserData> user_data_;
    };
}