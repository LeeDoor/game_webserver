#pragma once
#include <string>
#include "user.hpp"
#include "http_types.hpp"
namespace http_handler{

    struct PublicUser{
        PublicUser() = default;
        PublicUser(um::User ud){
            login = ud.login;
            password = ud.password;
        }
        um::Login login;
        um::Password password;
    };

}