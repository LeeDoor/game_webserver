#pragma once
#include "actor_id.hpp"
#include "nlohmann/json.hpp"
namespace game_manager{
    struct Event {
        using Ptr = std::shared_ptr<Event>;

        Event(int move_number, ActorId actor_id, std::string event)
            :move_number(move_number), actor_id(actor_id), event(event){}

        virtual void tojson(nlohmann::json& j) const;

        int move_number;
        ActorId actor_id;
        std::string event;
    };
}