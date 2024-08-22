#pragma once
#include "object_directed.hpp"
#include <functional>

namespace game_manager{
    class Bullet : public ObjectDirected {
    public: 
        using Ptr = std::shared_ptr<Bullet>;
        using DestroyFunc = std::function<void(ActorId)>;
        using FlyFunc = std::function<void(Direction)>;

        struct Methods{
            DestroyFunc destroy;
            FlyFunc fly;
        };

        Bullet(OwnerType owner, ActorId actor_id);
        Bullet(OwnerType owner, ActorId actor_id, Methods&& methods);
        bool operator==(Object::Ptr obj) const override;
        virtual void tojson(nlohmann::json& j) const;

        std::string UpdateTick() override;
    private:
        Methods methods_; 

        const std::string BULLET_DESTROY = "bullet_destroy";
        const std::string BULLET_FLY = "bullet_fly";
    };
}