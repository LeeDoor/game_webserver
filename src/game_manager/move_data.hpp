#pragma once
#include "direction.hpp"
#include "position.hpp"
#include <memory>

namespace game_manager{
    enum MoveType{
        Walk,
        Resign,
        PlaceBomb,
        PlaceGun,
    };
    struct MoveData{
        MoveType move_type;
        Position position;
        Direction direction;
    };
}