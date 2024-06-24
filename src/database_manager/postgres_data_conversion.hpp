#pragma once
#include <pqxx/pqxx>
#include "user_data.hpp"

namespace database_manager {
    static UserData from_result(const pqxx::row& row) {
        database_manager::UserData instance;
        instance.uuid = row.at("id").as<Uuid>();
        instance.login = row.at("login").as<Login>();
        instance.password = row.at("password").as<Password>();
        return instance;
    }
}
