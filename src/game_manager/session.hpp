#pragma once
#include <string>
#include <mutex>
#include <map>
#include <variant>
#include "user.hpp"
#include "state.hpp"
#include "event_manager.hpp"
#include "move_data.hpp"
#include "session_id.hpp"
#include "session_api_validator.hpp"

namespace game_manager{
    class Event;

    class Session : public std::enable_shared_from_this<Session>{
    public:
        using Ptr = std::shared_ptr<Session>;

        Session(um::Uuid player1, const um::Login& login1, um::Uuid player2, const um::Login& login2);

        static void InitApi();

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

        GameApiStatus ApiMove(const um::Uuid& player_id, MoveData md);
    private:
        /// @brief walking api. moves player_id to place_data
        void ApiWalk(Player::Ptr player, MoveData md);
        /// @brief resign api. resign as player_id
        void ApiResign(Player::Ptr player, MoveData md);
        /// @brief place bomb api. places player_id's bomb to place_data
        void ApiPlaceBomb(Player::Ptr player, MoveData md);
        void ApiPlaceGun(Player::Ptr player, MoveData md);

        void AfterMove();

        Player::Ptr player1(){return state_->players.front();}
        Player::Ptr player2(){return state_->players.back();}
        
        std::mutex move_mutex_;
        State::Ptr state_;

        um::Uuid player1_;
        um::Uuid player2_; 

        EventListWrapper::Ptr events_wrapper_;
        
        const std::map<um::Uuid, um::Login> uuid_to_login_;
        static std::map<MoveType, SessionApiValidator> api_validator_;

        const std::string PLAYER_WALK = "player_walk";
        const std::string PLAYER_RESIGN = "player_resign";
        const std::string PLAYER_PLACE_BOMB = "player_place_bomb";
        const std::string PLAYER_PLACE_GUN = "player_place_gun";
    };
}   
namespace gm = game_manager;