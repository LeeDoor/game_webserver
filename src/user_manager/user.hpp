#pragma once
#include <string>
#include "user_types.hpp"

namespace user_manager{
    struct User{
        Uuid uuid;
        Login login;
        Password password;
    };
}
namespace um = user_manager;
