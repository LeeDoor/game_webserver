#pragma once
#include "i_move_api.hpp"
#include "i_interaction_api.hpp"
#include "game_api_status.hpp"
#include <vector>
#include "session_api_director.hpp"
#include "state.hpp"
namespace gm = game_manager;

namespace game_manager {

    class Session : public IMoveApi, public IInteractionApi, public std::enable_shared_from_this<Session>{
    public:
        using Ptr = std::shared_ptr<Session>;
        
        Session();

        void UpdateStatePointers();
        static void InitApi();

    private:
        friend class SessionApiValidator;

        void Init(const Player::Id& login1, const Player::Id& login2) override;
        std::shared_ptr<const State> GetState() override;
        void SetState(State::Ptr state) override;
        std::shared_ptr<Player> GetCurrentPlayer() override;
        EventListWrapper::CPtr GetEvents() override;

        bool HasPlayer(const Player::Id& id) override;
        std::optional<Results> GetResults() override;
        GameApiStatus ApiMove(Player::Id id, MoveData md) override;
        void ApiWalk(Player::Ptr player, MoveData md) override;
        void ApiResign(Player::Ptr player, MoveData md) override;
        void ApiPlaceBomb(Player::Ptr player, MoveData md) override;
        void ApiPlaceGun(Player::Ptr player, MoveData md) override;

        void FinishSession(bool firstWinner) override;
        Bomb::Ptr PlaceBombObject(Position position, Object::OwnerType login) override;
        Gun::Ptr PlaceGunObject(Position position, Direction direction, Object::OwnerType login) override;
        Bullet::Ptr PlaceBulletObject(Position position, Direction direction, Object::OwnerType login) override;

        void RemoveObject(ActorId actor_id) override;
        void Explode(Position position) override;
        std::optional<std::list<IPlaceable::Ptr>> CollisionsOnCell(Bullet::Ptr bullet) override;

        void AfterMove();
        void IncreaseMoveNumber();
        bool ValidCell(Position position);

        Player::Ptr player1(){return state_->players.front();}
        Player::Ptr player2(){return state_->players.back();}

        ActorId GetId(){return id_counter_++;}
        ActorId id_counter_ = 0;

        State::Ptr state_;
        std::vector<std::weak_ptr<Player>> scoreboard_;  
        EventListWrapper::Ptr events_wrapper_;
        static std::map<MoveType, SessionApiValidator> api_validator_; 

        const std::string PLAYER_WALK = "player_walk";
        const std::string PLAYER_RESIGN = "player_resign";
        const std::string PLAYER_PLACE_BOMB = "player_place_bomb";
        const std::string PLAYER_PLACE_GUN = "player_place_gun";
    };

}