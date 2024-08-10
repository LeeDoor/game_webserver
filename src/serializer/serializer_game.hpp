#pragma once
#include <string>
#include <optional>
#include "../game_manager/session.hpp"

namespace serializer{
    std::string Serialize(const gm::State& state);
    std::string Serialize(const gm::Session::WalkData& wd);
    std::string Serialize(const gm::Session::PlaceBombData& pbd);

    std::optional<gm::State> DeserializeSessionState(const std::string& json);

    std::optional<gm::Session::MoveType> DefinePlayerMove(const std::string& json);
    std::optional<gm::Session::WalkData> DeserializeWalkData(const std::string& json);
    std::optional<gm::Session::PlaceBombData> DeserializePlaceBombData(const std::string& json);
    
}