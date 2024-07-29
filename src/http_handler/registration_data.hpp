#pragma once
#include <string>
#include "user.hpp"

namespace http_handler{
    struct RegistrationData{
        um::Login login;
        um::Password password;
    };

    bool ValidateRegData(const RegistrationData& rd);
    bool ValidateLogin(const um::Login& login);
    bool ValidatePassword(const um::Password& password);
}

namespace hh = http_handler;