#pragma once
#include "player.hpp"
#include "move_data.hpp"
#include "i_state.hpp"

namespace game_manager{
    class IMoveApi : virtual public IState { 
    public:
        using Ptr = std::shared_ptr<IMoveApi>;

        virtual void ApiWalk(Player::Ptr player, MoveData md) = 0;
        virtual void ApiResign(Player::Ptr player, MoveData md) = 0;
        virtual void ApiPlaceBomb(Player::Ptr player, MoveData md) = 0;
        virtual void ApiPlaceGun(Player::Ptr player, MoveData md) = 0;
    };
}