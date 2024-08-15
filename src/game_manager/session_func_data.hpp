#pragma once
#include "player.hpp"
#include <variant>

#define VARIANT_DATA_EMPTY true

namespace game_manager{

    /// @brief player's move data which have only placing info
    struct PlaceData{
        bool operator == (const PlaceData& other) const {
            return posX == other.posX && posY == other.posY;
        }

        Dimention posX;
        Dimention posY;
    };

    /// @brief player's move data which have placing and direction info
    struct DirectedPlaceData{
        bool operator == (const DirectedPlaceData& other) const {
            return posX == other.posX && posY == other.posY && direction == other.direction;
        }

        Direction direction;
        Dimention posX;
        Dimention posY;
    };
    struct NewObject{
        bool operator == (const NewObject& other) const = default;

        ActorId actor_id;
    };

    struct EmptyData{ 
        bool operator==(const EmptyData& other)const = default;
    };
    struct WalkData{
        bool operator==(const WalkData& other)const = default;
        PlaceData place;
    };
    struct BombData{
        bool operator==(const BombData& other)const = default;
        PlaceData place;
        NewObject new_object;
        unsigned ticks_left; 
    };
    using VariantEventData = std::variant<EmptyData, WalkData, BombData>;
}