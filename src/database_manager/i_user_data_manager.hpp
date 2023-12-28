#pragma once
#include "user_data.hpp"
#include <optional>
#include <memory>

namespace database_manager{
    class IUserDataManager{
    public:
        using Ptr = std::shared_ptr<IUserDataManager>;

        virtual bool AddLine(UserData& user_data) = 0;
        virtual std::optional<UserData> GetByUuid(const std::string& uuid) = 0;
        virtual std::optional<UserData> GetByLoginPassword(const std::string& login, const std::string& password) = 0;
    };
}