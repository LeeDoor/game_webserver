#pragma once
#include "event.hpp"
#include <list>

namespace game_manager{
    struct InteractionEvent : public Event{
        using Ptr = std::shared_ptr<InteractionEvent>;
        
        virtual void tojson(nlohmann::json& j) const;

        ActorId interacted_actor_id;
        std::list<Event::Ptr> happened;
    };
}