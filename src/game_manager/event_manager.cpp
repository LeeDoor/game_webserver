#include "event_manager.hpp"
#include <algorithm>
#include <stdexcept>

namespace game_manager{
    bool EventManager::AddEvent(Event&& event) {
        if(event.move_number < events_.size() ? events_.back().move_number : 1){
            throw std::logic_error("each new event's move should be more than or equal to previous(1 if first).");
        }
        
        if(event.move_number == events_.size() ? events_.back().move_number + 1 : 1){
            events_.push_back(std::move(event));
            from_move_.push_back(std::prev(events_.end()));
        }
        
        events_.push_back(std::move(event));
    }
    EventManager::Vec EventManager::GetEvents() {
        return events_;
    }
    EventManager::Vec EventManager::GetEvents(int from_move) {
        return Vec(from_move_[from_move], events_.end());
    }
}