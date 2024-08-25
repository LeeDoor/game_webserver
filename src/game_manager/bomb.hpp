#pragma once
#include "object.hpp"

namespace game_manager{
    class Bomb : public Object {
    public:
        using Ptr = std::shared_ptr<Bomb>;

        using ExplodeFunc = std::function<void(Position)>;

        struct Methods : public Object::Methods{
            ExplodeFunc explode;
        };

        Bomb(OwnerType owner, ActorId id);
        Bomb(OwnerType owner, ActorId id, Methods&& methods);
        bool operator==(Object::Ptr obj) const override;
        EventsType UpdateTick(int move_number) override;

        virtual void tojson(nlohmann::json& j) const;

        unsigned ticks_left = 3;

    private:
        ExplodeFunc explode_;

        const std::string BOMB_TICKING = "bomb_ticking";
        const std::string BOMB_EXPLODE = "bomb_explode";
    };
}