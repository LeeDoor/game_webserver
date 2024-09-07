#pragma once
#include <string>
#include <mutex>
#include <map>
#include <variant>
#include "user.hpp"
#include "event_manager.hpp"
#include "move_data.hpp"
#include "session_id.hpp"
#include "session_api_validator.hpp"
#include "i_move_api.hpp"

namespace game_manager{
    class Event;

    class Session : public std::enable_shared_from_this<Session>{
    public:
        using Ptr = std::shared_ptr<Session>;

        Session(um::Uuid player1, const um::Login& login1, um::Uuid player2, const um::Login& login2);

        static void InitApi();

        bool HasPlayer(const um::Uuid& uuid);
        State::CPtr GetState();
        void SetState(State&& state);
        EventListWrapper::CPtr GetEvents();

        struct ResultsUuid{
            um::Uuid winner;
            um::Uuid loser;
        };

        std::optional<ResultsUuid> GetResults();

        GameApiStatus ApiMove(const um::Uuid& player_id, MoveData md);
    private:

        Player::Ptr player1(){return state_->players.front();}
        Player::Ptr player2(){return state_->players.back();}
        
        std::mutex move_mutex_;
        IMoveApi::Ptr move_api_;

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