#pragma once
#include "player.hpp"
#include "object.hpp"
#include "obstacle.hpp"
#include <vector>
#include <list>
#include <memory>
#include <optional>

namespace game_manager {

    struct MapSize{
        bool operator == (const MapSize& other) const = default;

        unsigned width;
        unsigned height;
    };

    struct State{
        using Players = std::vector<Player::Ptr>;
        using Objects = std::list<Object::Ptr>;
        using Terrain = std::list<Obstacle::Ptr>;
        using NowTurn = Player::Login;
        using Ptr = std::shared_ptr<State>;
        using CPtr = std::shared_ptr<const State>;
        using OptCPtr = std::optional<CPtr>;
        
        bool operator==(const State& s) const {
            bool a = std::equal(players.begin(), players.end(), s.players.begin(), [](Player::Ptr a, Player::Ptr b){return *a == *b;});
            a = a && std::equal(terrain.begin(), terrain.end(), s.terrain.begin(), [](Obstacle::Ptr a, Obstacle::Ptr b){return *a == *b;});
            a = a && now_turn == s.now_turn;
            a = a && map_size == s.map_size;
            a = a && std::equal(objects.begin(), objects.end(), s.objects.begin(), [](Object::Ptr a, Object::Ptr b){return *a == b;});
            return a;
        }
        int move_number = 0;
        Players players;
        Objects objects;
        Terrain terrain;
        NowTurn now_turn;
        MapSize map_size;
    };

}