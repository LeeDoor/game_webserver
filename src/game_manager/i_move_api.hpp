#pragma once
#include "player.hpp"
#include "move_data.hpp"
#include "i_state.hpp"
#include "event_manager.hpp"
#include "game_api_status.hpp"

namespace game_manager{
    
    struct Results {
        um::Uuid winner;
        um::Uuid loser;
    };
    class IMoveApi : virtual public IState { 
    public:
        using Ptr = std::shared_ptr<IMoveApi>;

        virtual bool HasPlayer(um::Login login) = 0;
        virtual GameApiStatus ApiMove(um::Uuid uuid, MoveData md) = 0;
        virtual EventListWrapper::CPtr GetEvents() = 0;
        virtual std::optional<Results> GetResults() = 0;
    protected:
        virtual void ApiWalk(Player::Ptr player, MoveData md) = 0;
        virtual void ApiResign(Player::Ptr player, MoveData md) = 0;
        virtual void ApiPlaceBomb(Player::Ptr player, MoveData md) = 0;
        virtual void ApiPlaceGun(Player::Ptr player, MoveData md) = 0;
    };
}