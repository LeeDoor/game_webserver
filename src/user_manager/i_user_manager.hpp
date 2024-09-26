#pragma once
#include "user.hpp"
#include <optional>
#include <memory>

namespace user_manager{
    class IUserManager{
    public:
        using Ptr = std::shared_ptr<IUserManager>;

        virtual bool AddLine(const Login& login, const Password& password) = 0;
        virtual std::optional<User> GetByUuid(const Uuid& uuid) = 0;
        virtual std::optional<User> GetByLogin(const Login& login) = 0;
        virtual std::optional<User> GetByLoginPassword(const Login& login, const Password& password) = 0;
    };
}