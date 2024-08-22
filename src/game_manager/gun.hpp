#pragma once
#include "object_directed.hpp"
#include <functional>

namespace game_manager{
    class Gun : public ObjectDirected, public std::enable_shared_from_this<Gun> {
    public: 
        using Ptr = std::shared_ptr<Gun>;
        using ShootFunc = std::function<void(Gun::Ptr)>;

        struct Methods : public Object::Methods{
            ShootFunc shoot;
        };

        Gun(OwnerType owner, ActorId actor_id);
        Gun(OwnerType owner, ActorId actor_id, Methods&& methods);
        bool operator==(Object::Ptr obj) const override;
        virtual void tojson(nlohmann::json& j) const;

        EventsType UpdateTick(int move_number) override;
    
        unsigned ticks_to_shot = shot_cooldown_;
        unsigned shots_left = shot_amount_;
    private:
        ShootFunc shoot_;

        const unsigned shot_cooldown_ = 3;
        const unsigned shot_amount_ = 3;

        const std::string GUN_WAITING = "gun_waiting";
        const std::string GUN_SHOT = "gun_shot";
        const std::string GUN_SHOT_DESTROY = "gun_shot_destroy";
    };
}