#pragma once
#include "user.hpp"
#include <vector>
#include <memory>
#include <optional>

namespace game_manager {
    using Dimention = unsigned;

    struct Player{
        using Login = um::Login;

        bool operator == (const Player& other)const {
            return posX == other.posX && posY == other.posY && login == other.login;
        }

        Login login;
        Dimention posX;
        Dimention posY;
    };

    struct Obstacle {
        enum Type{
            Wall,
        };

        bool operator == (const Obstacle& other)const {
            return posX == other.posX && posY == other.posY && type == other.type;
        }

        Dimention posX;
        Dimention posY;
        Type type;
    };

    struct MapSize{
        unsigned width;
        unsigned height;
    };

    struct State{
        using Players = std::vector<Player>;
        using Terrain = std::vector<Obstacle>;
        using NowTurn = Player::Login;
        using Ptr = std::shared_ptr<State>;
        using CPtr = std::shared_ptr<const State>;
        using OptCPtr = std::optional<CPtr>;

        bool operator == (const State& state)const {
            return players == state.players && terrain == state.terrain && now_turn == state.now_turn;
        }

        Players players;
        Terrain terrain;
        NowTurn now_turn;
        MapSize map_size;
    };

}