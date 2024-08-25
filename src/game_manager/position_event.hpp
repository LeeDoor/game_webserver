#pragma once
#include "event.hpp"
#include "position.hpp"

namespace game_manager{
    struct PositionEvent : public Event{
        using Ptr = std::shared_ptr<PositionEvent>;
        PositionEvent(Event event, Position position)
            :Event(event), position(position){}

        virtual void tojson(nlohmann::json& j) const;

        Position position;
    };
}