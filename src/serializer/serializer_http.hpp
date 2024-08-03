#pragma once
#include <string>
#include <optional>
#include "public_user.hpp"
#include "registration_data.hpp"

namespace serializer{
    std::string Serialize(const hh::RegistrationData& rd);
    std::string Serialize(const hh::PublicUser& pud);

    std::optional<hh::RegistrationData> DeserializeRegData(const std::string& json);
    std::optional<hh::PublicUser> DeserializePublicUser(const std::string& json);
}