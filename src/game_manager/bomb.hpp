#pragma once
#include "object.hpp"
#include <functional>

namespace game_manager{
    class Bomb : public Object {
    public:
        using Ptr = std::shared_ptr<Bomb>;

        using ExplodeFunc = std::function<void(Dimention, Dimention)>;

        Bomb(OwnerType owner);
        Bomb(OwnerType owner, ExplodeFunc&& explode);
        bool operator==(Object::Ptr obj) const override;

        bool UpdateTick() override;

        unsigned ticks_left = 3;

    private:
        ExplodeFunc explode_;
    };
}