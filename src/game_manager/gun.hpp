#pragma once
#include "directed_object.hpp"
#include <functional>

namespace game_manager{
    class Gun : public DirectedObject, public std::enable_shared_from_this<Gun> {
    public: 
        using Ptr = std::shared_ptr<Gun>;
        using ShootFunc = std::function<ActorId(Gun::Ptr)>;

        struct Methods : public Object::Methods{
            ShootFunc shoot;
        };

        Gun(OwnerType owner, ActorId actor_id);
        Gun(OwnerType owner, ActorId actor_id, Methods&& methods);
        bool operator==(Object::Ptr obj) const override;
        virtual void tojson(nlohmann::json& j) const;

        EventsType UpdateTick(int move_number) override;
    
        unsigned ticks_to_shot;
        unsigned shots_left;
    private:
        ShootFunc shoot_;

        const unsigned shot_cooldown_;
        const unsigned shot_amount_;

        const std::string GUN_WAITING = "gun_waiting";
        const std::string GUN_DESTROY = "gun_destroy";
        const std::string GUN_SHOT = "gun_shot";
    };
}   