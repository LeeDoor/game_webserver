#pragma once
#include <string>
#include <optional>
#include "session_data.hpp"

namespace serializer{
    std::string Serialize(const session_manager::PublicSessionData& data);
    bool DefineSessionState(const std::string& json);
    std::optional<sm::PublicSessionData> DeserializePublicSessionData(const std::string& json);
}