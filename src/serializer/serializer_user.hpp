#pragma once
#include <string>
#include <optional>
#include <vector>
#include <map>
#include "user.hpp"
#include "token.hpp"

namespace serializer{
    namespace tm = token_manager;
    std::string Serialize(const um::User& ud);
    std::string Serialize(const std::vector<um::Uuid>& v);
    std::string Serialize(const std::map<tm::Token, um::Uuid>& ttu);

    std::optional<um::User> DeserializeUser(const std::string& json);
    std::optional<std::vector<um::Uuid>> DeserializeUuids(const std::string& json);
}