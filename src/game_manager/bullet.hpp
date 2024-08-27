#pragma once
#include "directed_object.hpp"
#include <functional>

namespace game_manager{
    class Bullet : public DirectedObject, public std::enable_shared_from_this<Bullet> {
    public: 
        using Ptr = std::shared_ptr<Bullet>;
        using GetCollisionsFunc = std::function<std::optional<std::list<IPlaceable::Ptr>>(Bullet::Ptr)>;

        struct Methods : public Object::Methods{
            GetCollisionsFunc get_collisions;
        };

        Bullet(OwnerType owner, ActorId actor_id);
        Bullet(OwnerType owner, ActorId actor_id, Methods&& methods);
        bool operator==(Object::Ptr obj) const override;
        virtual void tojson(nlohmann::json& j) const;

        EventsType UpdateTick(int move_number) override;
    private:
        GetCollisionsFunc get_collisions_;

        const std::string BULLET_DESTROY = "bullet_destroy";
        const std::string BULLET_FLY = "bullet_fly";
    };
}