#include "event.hpp"
#include "position_event.hpp"
#include "new_object_event.hpp"
#include "new_dir_object_event.hpp"
#include "interaction_event.hpp"
#include "type_serializer.hpp"

namespace game_manager{
    void Event::tojson(nlohmann::json& j) const {
        j["move_number"] = move_number;
        j["actor_id"] = actor_id;
        j["event"] = event;
    }
    void PositionEvent::tojson(nlohmann::json& j) const {
        Event::tojson(j);
        j["position"] = position;
    }

    void NewObjectEvent::tojson(nlohmann::json& j) const {
        PositionEvent::tojson(j);
        j["new_actor_id"] = new_actor_id;
    }

    void NewDirObjectEvent::tojson(nlohmann::json& j) const {
        NewObjectEvent::tojson(j);
        j["direction"] = direction;
    }
    
    void InteractionEvent::tojson(nlohmann::json& j) const {
        Event::tojson(j);
        j["interacted_actor_id"] = interacted_actor_id;
        j["happened"] = happened;
    }
}