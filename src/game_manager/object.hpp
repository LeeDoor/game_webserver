#pragma once
#include <memory>
#include "player.hpp"

namespace game_manager{
    struct State;

    class Object{
    public:
        using Ptr = std::shared_ptr<Object>;
        using OwnerType = Player::Login;
        using StateType = std::shared_ptr<State>;

        Object(OwnerType owner, StateType state);
        Object(OwnerType owner);
        virtual ~Object();
        virtual bool operator==(Object::Ptr obj) const;
        void SetState(StateType state);

        virtual void Place(Dimention posX, Dimention posY);
        virtual bool UpdateTick() = 0;
        
        Dimention posX, posY;
        OwnerType owner;
    protected:
        StateType state_;
    };
}