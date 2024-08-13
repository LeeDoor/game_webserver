#pragma once
#include <string>
#include <vector>
#include "player.hpp"
#include "session_func_data.hpp"
namespace game_manager{
    struct Event {
        bool operator == (const Event& other) const {
            return actor_id == other.actor_id && 
                event_type == other.event_type &&
                data == other.data && 
                move_number == other.move_number;
        }

        int move_number;
        ActorId actor_id;
        std::string event_type;
        VariantData data;
    };

    class EventManager{
    public:
        using Vec = std::vector<Event>;

        bool AddEvent(Event&& event);
        Vec GetEvents();
        Vec GetEvents(int from_move);

    private:
        std::vector<Vec::iterator> from_move_;
        Vec events_;
    };
}