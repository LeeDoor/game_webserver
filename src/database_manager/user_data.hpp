#pragma once
#include <boost/uuid/uuid.hpp>
#include <string>

namespace database_manager{
    namespace ids = boost::uuids;
    struct UserData{
        ids::uuid uuid;
        std::string login;
        std::string password;
    };
}
namespace dm = database_manager;
