#pragma once
#include "player.hpp"
#include "object.hpp"
#include <vector>
#include <memory>
#include <optional>

namespace game_manager {
    struct Obstacle {
        enum Type{
            Wall,
        };

        bool operator == (const Obstacle& other) const = default;

        Dimention posX;
        Dimention posY;
        Type type;
    };

    struct MapSize{
        bool operator == (const MapSize& other) const = default;

        unsigned width;
        unsigned height;
    };
    struct State{
        using Players = std::vector<Player>;
        using Objects = std::vector<Object::Ptr>;
        using Terrain = std::vector<Obstacle>;
        using NowTurn = Player::Login;
        using Ptr = std::shared_ptr<State>;
        using CPtr = std::shared_ptr<const State>;
        using OptCPtr = std::optional<CPtr>;
        
        bool operator==(const State& s) const {
            return players == s.players && 
                terrain == s.terrain && 
                now_turn == s.now_turn && 
                map_size == s.map_size &&
                std::equal(objects.begin(), objects.end(), s.objects.begin(), [](Object::Ptr a, Object::Ptr b){return *a == b;});
        }
        int move_number = 0;
        Players players;
        Objects objects;
        Terrain terrain;
        NowTurn now_turn;
        MapSize map_size;
    };

}