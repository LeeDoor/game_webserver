#pragma once
#include <string>
#include <optional>
#include "../game_manager/session.hpp"

namespace serializer{
    std::string Serialize(const gm::State& state);
    std::string Serialize(const gm::MoveData& md);
    std::string Serialize(const gm::EventListWrapper::Vec& elwv);

    std::optional<gm::State> DeserializeSessionState(const std::string& json);
    std::optional<int> DeserializeFromMove(const std::string& json);

    std::optional<gm::MoveData> DeserializeMoveData(const std::string& json);

}