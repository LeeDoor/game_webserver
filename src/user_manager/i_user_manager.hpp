#pragma once
#include "user.hpp"
#include "registration_data.hpp"
#include <optional>
#include <memory>

namespace user_manager{
    class IUserManager{
    public:
        using Ptr = std::shared_ptr<IUserManager>;

        virtual bool AddLine(hh::RegistrationData& rd) = 0;
        virtual std::optional<User> GetByUuid(const Uuid& uuid) = 0;
        virtual std::optional<User> GetByLoginPassword(const Login& login, const Password& password) = 0;
    };
}