#pragma once
#include "object.hpp"
#include <functional>

namespace game_manager{
    class Bomb : public Object {
    public:
        using Ptr = std::shared_ptr<Bomb>;

        using ExplodeFunc = std::function<void(Dimention, Dimention)>;

        Bomb(OwnerType owner, ActorId id);
        Bomb(OwnerType owner, ActorId id, ExplodeFunc&& explode);
        bool operator==(Object::Ptr obj) const override;

        std::pair<std::string, bool> UpdateTick() override;

        unsigned ticks_left = 3;

    private:
        ExplodeFunc explode_;

        const std::string BOMB_TICKING = "bomb_ticking";
        const std::string BOMB_EXPLODE = "bomb_explode";
    };
}