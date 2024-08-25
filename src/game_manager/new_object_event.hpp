#pragma once
#include "position_event.hpp"

namespace game_manager{
    struct NewObjectEvent : public PositionEvent{
        using Ptr = std::shared_ptr<NewObjectEvent>;
        
        NewObjectEvent(PositionEvent pos_event, ActorId new_actor_id)
            :PositionEvent(pos_event), new_actor_id(new_actor_id){}

        virtual void tojson(nlohmann::json& j) const;

        ActorId new_actor_id;
    };
}