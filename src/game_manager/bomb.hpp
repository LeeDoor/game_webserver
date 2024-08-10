#pragma once
#include "object.hpp"

namespace game_manager{
    class Bomb : public Object {
    public:
        using Ptr = std::shared_ptr<Bomb>;

        Bomb(OwnerType owner, StateType state);
        Bomb(OwnerType owner);
        bool operator==(Object::Ptr obj) const override;

        bool UpdateTick() override;

        unsigned ticks_left = 3;
    };
}