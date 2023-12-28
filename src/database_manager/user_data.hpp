#pragma once
#include <string>

namespace database_manager{
    struct UserData{
        std::string uuid;
        std::string login;
        std::string password;
    };
}
namespace dm = database_manager;
