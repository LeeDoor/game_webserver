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
    using VariantData = std::variant<bool, PlaceData, DirectedPlaceData>;
}