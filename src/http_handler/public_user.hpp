#pragma once
#include <string>
#include "user.hpp"
#include "http_types.hpp"
namespace http_handler{

    struct PublicUser{
        PublicUser() = default;
        PublicUser(dm::User ud){
            login = ud.login;
            password = ud.password;
        }
        dm::Login login;
        dm::Password password;
    };

}