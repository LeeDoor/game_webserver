#pragma once
#include <nlohmann/json.hpp>
#include "registration_data.hpp"

namespace user_data{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RegistrationData, login, password)
}