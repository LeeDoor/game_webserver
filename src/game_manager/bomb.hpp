#pragma once
#include "object_placed.hpp"
#include <functional>

namespace game_manager{
    class Bomb : public ObjectPlaced {
    public:
        using Ptr = std::shared_ptr<Bomb>;

        using ExplodeFunc = std::function<void(Dimention, Dimention)>;

        struct Methods : public Object::Methods{
            ExplodeFunc explode;
        };

        Bomb(OwnerType owner, ActorId id);
        Bomb(OwnerType owner, ActorId id, Methods&& methods);
        bool operator==(Object::Ptr obj) const override;
        virtual void tojson(nlohmann::json& j) const;

        EventsType UpdateTick(int move_number) override;

        unsigned ticks_left = 3;

    private:
        ExplodeFunc explode_;

        const std::string BOMB_TICKING = "bomb_ticking";
        const std::string BOMB_EXPLODE = "bomb_explode";
    };
}