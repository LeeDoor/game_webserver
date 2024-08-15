#include "event_manager.hpp"
#include <algorithm>
#include <stdexcept>

namespace game_manager{
    void EventListWrapper::Clear(){
        from_move_.clear();
        value_.clear();
    }

    const EventListWrapper::Vec& EventListWrapper::Value()const {
        return value_;
    }
    void EventListWrapper::AddEvent(Event&& event) {
        if(event.move_number < (value_.size() ? value_.back().move_number : 1)){
            throw std::logic_error("each new event's move should be more than or equal to previous(1 if first).");
        }
        
        if(event.move_number == (value_.size() ? value_.back().move_number + 1 : 1)){
            from_move_.push_back(value_.size());
        }
        
        value_.push_back(std::move(event));
    }
    EventListWrapper::Vec EventListWrapper::FromMove(int from_move) const {
        // from_move starts from zero, need to decrement
        return Vec(std::next(value_.begin(), from_move_[from_move - 1]), value_.end());
    }
}