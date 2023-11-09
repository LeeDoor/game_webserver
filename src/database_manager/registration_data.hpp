#pragma once
#include <string>

namespace http_handler{
    struct RegistrationData{
        std::string login;
        std::string password;
    };

    bool ValidateRegData(const RegistrationData& rd);
    bool ValidateLogin(const std::string& login);
    bool ValidatePassword(const std::string& password);
}