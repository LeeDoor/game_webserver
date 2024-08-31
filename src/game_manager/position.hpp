#pragma once
namespace game_manager{
    using Dimention = unsigned;
    struct Position {
        bool operator==(const Position& other) const = default;
        Position() : Position(0,0){}
        Position(Dimention x, Dimention y)
            :x(x), y(y){}
        Dimention x;
        Dimention y;
    };

}