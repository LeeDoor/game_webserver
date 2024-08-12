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

    struct Player{
        using Login = um::Login;

        bool operator == (const Player& other)const {
            return posX == other.posX && posY == other.posY && login == other.login;
        }

        Login login;
        Dimention posX;
        Dimention posY;
    };
}