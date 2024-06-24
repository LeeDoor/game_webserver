#pragma once
#include <string>
#include "user_data_types.hpp"

namespace database_manager{
    struct UserData{
        Uuid uuid;
        Login login;
        Password password;
    };
}
namespace dm = database_manager;
