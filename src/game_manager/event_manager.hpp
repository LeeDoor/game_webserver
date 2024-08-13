#pragma once
#include <string>
#include <vector>
#include <list>
#include <optional>
#include <memory>
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

    class EventListWrapper{
    public:
        using Ptr = std::shared_ptr<EventListWrapper>;
        using CPtr = std::shared_ptr<const EventListWrapper>;
        using OptCPtr = std::optional<CPtr>;
        using Vec = std::list<Event>;
        const Vec& Value() const;
        
        void AddEvent(Event&& event);
        Vec FromMove(int move_number) const;
    private:
        std::vector<int> from_move_;
        Vec value_;
    };
}