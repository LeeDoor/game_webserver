#pragma once
#include "event.hpp"
#include <list>

namespace game_manager{
    struct InteractionEvent : public Event{
        using Ptr = std::shared_ptr<InteractionEvent>;
        
        InteractionEvent(Event event, ActorId interacted_actor_id, std::list<Event::Ptr> happened)
            :Event(event), interacted_actor_id(interacted_actor_id), happened(happened){}
        virtual void tojson(nlohmann::json& j) const;

        ActorId interacted_actor_id;
        std::list<Event::Ptr> happened;
    };
}