#pragma once
#include <memory>
#include "player.hpp"

namespace game_manager{
    struct State;

    class Object{
    public:
        using Ptr = std::shared_ptr<Object>;
        using OwnerType = Player::Login;
        using StateType = State&;

        Object(OwnerType owner, StateType state);
        virtual ~Object();

        virtual void Place(Dimention posX, Dimention posY);
        virtual bool UpdateTick() = 0;
        
        Dimention posX, posY;
        OwnerType owner;
    protected:
        StateType& state_;
    };
}