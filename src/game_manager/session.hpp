#pragma once
#include <string>
#include <mutex>
#include <map>
#include <variant>
#include "user.hpp"
#include "state.hpp"
#include "event_manager.hpp"
#include "bomb.hpp"
#include "gun.hpp"
#include "bullet.hpp"
#include "move_data.hpp"
#include "session_id.hpp"

namespace game_manager{
    class Event;

    class Session {
    public:
        using Ptr = std::shared_ptr<Session>;

        Session(um::Uuid player1, const um::Login& login1, um::Uuid player2, const um::Login& login2);

        /// @brief check if given player is in the session
        /// @param uuid given uuid 
        /// @return true if the player is in here
        bool HasPlayer(const um::Uuid& uuid);
        /// @brief get state of the session
        /// @return const pointer to state object
        State::CPtr GetState();
        void SetState(State&& state);

        /// @brief get event list from given move
        /// @return returns all events happened from given move (including).
        EventListWrapper::CPtr GetEvents();

        /// @brief game result data about finished session
        struct ResultsUuid{
            um::Uuid winner;
            um::Uuid loser;
        };

        /// @brief get session result data object if finished (std::nullopt overwise)
        std::optional<ResultsUuid> GetResults();

        /// @brief type of player's move
        enum MoveType{
            Walk,
            Resign,
            PlaceBomb,
        };
        
        /// @brief player's move status
        enum GameApiStatus{
            Ok,
            WrongMove,
            NotYourMove,
        };
        
        /// @brief walking api. moves player_id to place_data
        GameApiStatus ApiWalk(const um::Uuid& player_id, PosMoveData data);
        /// @brief resign api. resign as player_id
        GameApiStatus ApiResign(const um::Uuid& player_id);
        /// @brief place bomb api. places player_id's bomb to place_data
        GameApiStatus ApiPlaceBomb(const um::Uuid& player_id, PosMoveData data);
    private:
        /// @brief sets map with login and uuid for both players, sets default map settings like obstacles.
        /// doesnt matter who is first and who is second player passed. function declares who is moving first.
        /// @param login1 first player
        /// @param login2 second player
        void InitSessionState(const um::Login& login1, const um::Login& login2);
        /// @brief finishes session and sets value to results_.
        /// session ending should be handled by game_manager.
        /// @param firstWinner true if first player won.
        void FinishSession(bool firstWinner);
        /// @brief makes changes after each player's move. should be called at the end of API functions.
        void AfterMove();

        // object placement
        Bomb::Ptr PlaceBombObject(Position position, Object::OwnerType login);
        Gun::Ptr PlaceGunObject(Position position, Direction direction, Object::OwnerType login);
        Bullet::Ptr PlaceBulletObject(Position position, Direction direction, Object::OwnerType login);
        
        /// @brief removes given object from scene.
        void RemoveObject(ActorId actor_id) ;

        // object acting
        void Explode(Position position);
        void ShootBullet(Gun::Ptr gun);

        // assisting functions

        /// @brief returns true if given cell is valid to walk on or to place an object.
        bool ValidCell(Position position);

        std::mutex move_mutex_;
        State::Ptr state_;

        um::Uuid player1_;
        um::Uuid player2_;

        // doesnt require free()
        std::vector<Player*> scoreboard_;   

        State::Objects& objects(){return state_->objects;}
        Player& player1(){return *state_->players.front();}
        Player& player2(){return *state_->players.back();}
        um::Login& nowTurn(){return state_->now_turn;}
        State::Terrain& terrain(){return state_->terrain;}

        const std::map<um::Uuid, um::Login> uuid_to_login_;
        
        EventListWrapper::Ptr events_wrapper_;

        ActorId GetId(){return id_counter_++;}
        // counter to create objects with new id
        ActorId id_counter_ = 0;

        const std::string PLAYER_WALK = "player_walk";
        const std::string PLAYER_RESIGN = "player_resign";
        const std::string PLAYER_PLACE_BOMB = "player_place_bomb";
        const std::string PLAYER_WON = "player_won";
    };
}   
namespace gm = game_manager;