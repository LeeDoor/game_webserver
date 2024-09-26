#pragma once
#include "actor_id.hpp"
#include "event_manager.hpp"

namespace game_manager{
    using EventsType = EventListWrapper::Vec;
    class Bullet;
    class IActor{
    public:
        IActor() : IActor(0) {}
        IActor(ActorId ai) : actor_id(ai) {}

        bool operator ==(const IActor& other) const = default;
        
        virtual EventsType InteractWithBullet(std::shared_ptr<Bullet> bullet) {
            return {};
        }

        ActorId actor_id;
    };
}
