#pragma once
#include <pqxx/pqxx>
#include "user_data.hpp"

namespace database_manager {
    static UserData from_result(const pqxx::row& row) {
        database_manager::UserData instance;
        instance.uuid = row.at("id").as<std::string>();
        instance.login = row.at("login").as<std::string>();
        instance.password = row.at("password").as<std::string>();
        return instance;
    }
}
