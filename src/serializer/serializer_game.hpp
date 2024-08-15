#pragma once
#include <string>
#include <optional>
#include "../game_manager/session.hpp"

namespace serializer{
    std::string Serialize(const gm::State& state);
    std::string Serialize(const gm::PlaceData& wd);
    std::string Serialize(const gm::EventListWrapper::Vec& elwv);

    std::optional<gm::State> DeserializeSessionState(const std::string& json);
    std::optional<int> DeserializeFromMove(const std::string& json);

    std::optional<gm::Session::MoveType> DefinePlayerMove(const std::string& json);
    std::optional<gm::PlaceData> DeserializePlaceData(const std::string& json);
    std::optional<gm::Session::VariantApiData> DeserializeMoveData(const std::string& json, gm::Session::MoveType mt);
    
}