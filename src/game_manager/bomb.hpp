#pragma once
#include "object.hpp"

namespace game_manager{
    class Bomb : public Object {
    public:
        using Ptr = std::shared_ptr<Bomb>;

        Bomb(OwnerType owner, ActorId id);
        bool operator==(Object::Ptr obj) const override;
        EventsType UpdateTick(int move_number) override;

        virtual void tojson(nlohmann::json& j) const;

        unsigned ticks_left = 3;

    private:
        const std::string BOMB_TICKING = "bomb_ticking";
        const std::string BOMB_EXPLODE = "bomb_explode";
    };
}