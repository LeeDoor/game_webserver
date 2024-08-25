#pragma once
#include "directed_object.hpp"
#include <functional>

namespace game_manager{
    class Bullet : public DirectedObject, public std::enable_shared_from_this<Bullet> {
    public: 
        using Ptr = std::shared_ptr<Bullet>;

        struct Methods : public Object::Methods{};

        Bullet(OwnerType owner, ActorId actor_id);
        Bullet(OwnerType owner, ActorId actor_id, Methods&& methods);
        bool operator==(Object::Ptr obj) const override;
        virtual void tojson(nlohmann::json& j) const;

        EventsType UpdateTick(int move_number) override;
    private:
        const std::string BULLET_DESTROY = "bullet_destroy";
        const std::string BULLET_FLY = "bullet_fly";
    };
}