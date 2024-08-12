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

        Object(OwnerType owner, ActorId id);
        virtual ~Object();
        virtual bool operator==(Object::Ptr obj) const;
        void SetState(StateType state);

        virtual void Place(Dimention posX, Dimention posY);
        /// @brief updates object's tick
        /// @return string is an event type, bool = true if object need to be destroyed
        virtual std::pair<std::string, bool> UpdateTick() = 0;
        
        ActorId id;
        Dimention posX, posY;
        OwnerType owner;        
    };
}