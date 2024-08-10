#pragma once
#include "object.hpp"

namespace game_manager{
    class Bomb : public Object {
    public:
        using Ptr = std::shared_ptr<Bomb>;

        Bomb(OwnerType owner, StateType state);

        bool UpdateTick() override;

        unsigned ticks_left = 3;
    };
}