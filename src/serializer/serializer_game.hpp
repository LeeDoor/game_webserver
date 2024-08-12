#pragma once
#include <string>
#include <optional>
#include "../game_manager/session.hpp"

namespace serializer{
    std::string Serialize(const gm::State& state);
    std::string Serialize(const gm::Session::PlaceData& wd);

    std::optional<gm::State> DeserializeSessionState(const std::string& json);

    std::optional<gm::Session::MoveType> DefinePlayerMove(const std::string& json);
    std::optional<gm::Session::PlaceData> DeserializePlaceData(const std::string& json);
    
}