#pragma once
#include "object_directed.hpp"
#include <functional>

namespace game_manager{
    class Bullet : public ObjectDirected, public std::enable_shared_from_this<Bullet> {
    public: 
        using Ptr = std::shared_ptr<Bullet>;
        using GetShootablesFunc = std::function<std::optional<IShootable::Arr>(Bullet::Ptr)>;

        struct Methods : public Object::Methods{
            GetShootablesFunc get_shootables;
        };

        Bullet(OwnerType owner, ActorId actor_id);
        Bullet(OwnerType owner, ActorId actor_id, Methods&& methods);
        bool operator==(Object::Ptr obj) const override;
        virtual void tojson(nlohmann::json& j) const;

        EventsType UpdateTick(int move_number) override;
    private:
        GetShootablesFunc get_shootables_;

        const std::string BULLET_DESTROY = "bullet_destroy";
        const std::string BULLET_FLY = "bullet_fly";
    };
}