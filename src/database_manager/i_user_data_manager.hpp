#pragma once
#include "user_data.hpp"
#include "registration_data.hpp"
#include <optional>
#include <memory>

namespace database_manager{
    class IUserDataManager{
    public:
        using Ptr = std::shared_ptr<IUserDataManager>;

        virtual bool AddLine(hh::RegistrationData& rd) = 0;
        virtual std::optional<UserData> GetByUuid(const Uuid& uuid) = 0;
        virtual std::optional<UserData> GetByLoginPassword(const Login& login, const Password& password) = 0;
    };
}