#pragma once
#include <string>
#include "user_data.hpp"

namespace http_handler{
    struct RegistrationData{
        dm::Login login;
        dm::Password password;
    };

    bool ValidateRegData(const RegistrationData& rd);
    bool ValidateLogin(const dm::Login& login);
    bool ValidatePassword(const dm::Password& password);
}

namespace hh = http_handler;