#pragma once
#include <pqxx/pqxx>
#include "user.hpp"

namespace user_manager {
    static User from_result(const pqxx::row& row) {
        user_manager::User instance;
        instance.uuid = row.at("id").as<Uuid>();
        instance.login = row.at("login").as<Login>();
        instance.password = row.at("password").as<Password>();
        return instance;
    }
}
