#pragma once
#include "player.hpp"
#include "object.hpp"
#include "i_shootable.hpp"
#include <vector>
#include <list>
#include <memory>
#include <optional>

namespace game_manager {
    class Obstacle : public IShootable {
    public:
        using Ptr = std::shared_ptr<Obstacle>;

        enum Type{
            Wall,
        };

        Obstacle(){}
        Obstacle(Dimention posX, Dimention posY, Type type):
            posX(posX), posY(posY), type(type){}

        bool operator == (const Obstacle& other) const {
            return posX == other.posX && posY == other.posY && type == other.type;
        }
        EventListWrapper::Vec GetShot(int move_number, std::shared_ptr<Bullet>) override {return {};} 

        Dimention posX = 0;
        Dimention posY = 0;
        Type type = Type::Wall;
    };

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
            return std::equal(players.begin(), players.end(), s.players.begin(), [](Player::Ptr a, Player::Ptr b){return *a == *b;}) && 
                std::equal(terrain.begin(), terrain.end(), s.terrain.begin(), [](Obstacle::Ptr a, Obstacle::Ptr b){return *a == *b;}) && 
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