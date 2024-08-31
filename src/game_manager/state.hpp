#pragma once
#include "player.hpp"
#include "object.hpp"
#include "obstacle.hpp"
#include "bomb.hpp"
#include "gun.hpp"
#include "bullet.hpp"
#include <vector>
#include <list>
#include <memory>
#include <optional>
namespace gm = game_manager;

namespace game_manager {

    struct MapSize{
        bool operator == (const MapSize& other) const = default;

        Dimention width;
        Dimention height;
    };

    class State : public std::enable_shared_from_this<State>{
    public:
        using Players = std::vector<Player::Ptr>;
        using Objects = std::list<Object::Ptr>;
        using Terrain = std::list<Obstacle::Ptr>;
        using NowTurn = Player::Login;
        using Ptr = std::shared_ptr<State>;
        using CPtr = std::shared_ptr<const State>;
        using OptCPtr = std::optional<CPtr>;
        
        State();

        bool operator==(const State& s) const;

        void UpdateStatePointers();

        /// @brief sets map with login and uuid for both players, sets default map settings like obstacles.
        /// doesnt matter who is first and who is second player passed. function declares who is moving first.
        /// @param login1 first player
        /// @param login2 second player
        void Init(const um::Login& login1, const um::Login& login2);

        /// @brief makes changes after each player's move. should be called at the end of API functions.
        void AfterMove();

        /// @brief returns true if given cell is valid to walk on or to place an object.
        bool ValidCell(Position position);

        void FinishSession(bool firstWinner);
        Bomb::Ptr PlaceBombObject(Position position, Object::OwnerType login);
        Gun::Ptr PlaceGunObject(Position position, Direction direction, Object::OwnerType login);
        Bullet::Ptr PlaceBulletObject(Position position, Direction direction, Object::OwnerType login);

        void RemoveObject(ActorId actor_id);
        void Explode(Position position);
        std::optional<std::list<IPlaceable::Ptr>> CollisionsOnCell(Bullet::Ptr bullet);

        Player::Ptr player1(){return players.front();}
        Player::Ptr player2(){return players.back();}

        ActorId GetId(){return id_counter_++;}
        // counter to create objects with new id
        ActorId id_counter_ = 0;

        // doesnt require free()
        std::vector<std::weak_ptr<Player>> scoreboard_;  

        int move_number = 0;
        Players players;
        Objects objects;
        Terrain terrain;
        NowTurn now_turn;
        MapSize map_size;
        
        EventListWrapper::Ptr events_wrapper_;
    };

}