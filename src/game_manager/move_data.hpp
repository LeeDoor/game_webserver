#pragma once
#include "direction.hpp"
#include "position.hpp"
#include <memory>

namespace game_manager{

    struct MoveData{
        using Ptr = std::shared_ptr<MoveData>;
        virtual ~MoveData() = default;
    };
    struct PosMoveData : public MoveData{
        Position position;
    };
    struct DirPosMoveData : public PosMoveData{
        Direction direction;
    };
}