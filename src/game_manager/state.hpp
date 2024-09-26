#pragma once

#include "player.hpp"
#include "bomb.hpp"
#include "gun.hpp"
#include "bullet.hpp"
#include "object.hpp"
#include "obstacle.hpp"

namespace game_manager {

    template<template <typename> typename Container, typename Type>
    bool comp_cont(Container<std::shared_ptr<Type>> c1, Container<std::shared_ptr<Type>> c2){
        return std::equal(c1.begin(), c1.end(), c2.begin(), [](std::shared_ptr<Type> a, std::shared_ptr<Type> b){ return *a == *b; });
    }
    template<template <typename> typename Container, typename Type>
    bool comp_cont_vtable(Container<std::shared_ptr<Type>> c1, Container<std::shared_ptr<Type>> c2){
        return std::equal(c1.begin(), c1.end(), c2.begin(), [](std::shared_ptr<Type> a, std::shared_ptr<Type> b){ return *a == b; });
    }
    struct MapSize{
        bool operator == (const MapSize& other) const = default;

        Dimention width;
        Dimention height;
    };
    struct State {
        using Ptr = std::shared_ptr<State>;
        using CPtr = std::shared_ptr<const State>;
        using OptCPtr = std::optional<CPtr>;

        using Players = std::vector<Player::Ptr>;
        using Objects = std::list<Object::Ptr>;
        using Terrain = std::list<Obstacle::Ptr>;
        using NowTurn = Player::Id;

        bool operator==(const State& s) const {
            return comp_cont(players, s.players) &&
                comp_cont_vtable(objects, s.objects) &&
                comp_cont(terrain, s.terrain) &&
                now_turn == s.now_turn &&
                map_size == s.map_size;
        }
        int move_number = 0;
        Players players;
        Objects objects;
        Terrain terrain;
        NowTurn now_turn;
        MapSize map_size;
    };
}