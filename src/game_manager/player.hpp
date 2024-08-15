#pragma once
#include "user.hpp"

namespace game_manager{
    using Dimention = unsigned;
    enum Direction {
        Up,
        Right,
        Down,
        Left
    };
    using ActorId = int;
    struct Player{
        using Login = um::Login;

        bool operator == (const Player& other) const = default;

        ActorId id;
        Login login;
        Dimention posX;
        Dimention posY;
    };
}