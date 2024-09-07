#pragma once
#include "player.hpp"
#include "object.hpp"
#include "obstacle.hpp"
#include "i_move_api.hpp"
#include "i_interaction_api.hpp"
#include <vector>
namespace gm = game_manager;

namespace game_manager {

    struct MapSize{
        bool operator == (const MapSize& other) const = default;

        Dimention width;
        Dimention height;
    };

    class State : public IMoveApi, public IInteractionApi, public std::enable_shared_from_this<State>{
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
    private:
        void Init(const um::Login& login1, const um::Login& login2) override;
        std::shared_ptr<const State> GetState() override;
        void SetState(State&& state) override;
        std::shared_ptr<Player> GetCurrentPlayer() override;

        void ApiWalk(Player::Ptr player, MoveData md) override;
        void ApiResign(Player::Ptr player, MoveData md) override;
        void ApiPlaceBomb(Player::Ptr player, MoveData md) override;
        void ApiPlaceGun(Player::Ptr player, MoveData md) override;

        /// @brief makes changes after each player's move. should be called at the end of API functions.
        void AfterMove();

        void IncreaseMoveNumber();

        /// @brief returns true if given cell is valid to walk on or to place an object.
        bool ValidCell(Position position);

        void FinishSession(bool firstWinner) override;
        Bomb::Ptr PlaceBombObject(Position position, Object::OwnerType login) override;
        Gun::Ptr PlaceGunObject(Position position, Direction direction, Object::OwnerType login) override;
        Bullet::Ptr PlaceBulletObject(Position position, Direction direction, Object::OwnerType login) override;

        void RemoveObject(ActorId actor_id) override;
        void Explode(Position position) override;
        std::optional<std::list<IPlaceable::Ptr>> CollisionsOnCell(Bullet::Ptr bullet) override;

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