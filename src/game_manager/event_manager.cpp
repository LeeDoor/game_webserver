#include "event_manager.hpp"
#include <algorithm>
#include <stdexcept>

namespace game_manager{
    void EventListWrapper::Clear(){
        from_move_.clear();
        value_.clear();
        move_number_ = 0;
    }

    const EventListWrapper::Vec& EventListWrapper::Value()const {
        return value_;
    }
    void EventListWrapper::AddEvent(Event::Ptr event) {
        event->move_number = move_number_;
        if(event->move_number < (value_.size() ? value_.back()->move_number : 1)){
            throw std::logic_error("each new event's move should be more than or equal to previous(1 if first).");
        }
        
        if(event->move_number == (value_.size() ? value_.back()->move_number + 1 : 1)){
            from_move_.push_back(value_.size());
        }
        
        value_.push_back(event);
    }
    void EventListWrapper::AddEvents(Vec&& events) {
        for(auto event : events){
            // no SPLICE, from_move_ needs to be edited
            AddEvent(event);
        }
    }
    EventListWrapper::Vec EventListWrapper::FromMove(int from_move) const {
        // from_move starts from one, need to decrement
        if(from_move < 1)
            return value_;
        if(from_move > from_move_.size())
            return {};
        return Vec(std::next(value_.begin(), from_move_[from_move - 1]), value_.end());
    }
    void EventListWrapper::SetMoveNumber(int mn){
        move_number_ = mn;
    }
}