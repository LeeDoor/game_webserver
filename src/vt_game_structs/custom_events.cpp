#include "custom_events.hpp"

namespace game_manager{
    PositionEvent::Ptr WalkEvent(PositionEvent e){
        return std::make_shared<PositionEvent>(e);
    }

    NewObjectEvent::Ptr BombEvent(NewObjectEvent e){
        return std::make_shared<NewObjectEvent>(e);
    }
    
    NewDirObjectEvent::Ptr GunEvent(NewDirObjectEvent e){
        return std::make_shared<NewDirObjectEvent>(e);
    }

    NewDirObjectEvent::Ptr BulletEvent(NewDirObjectEvent e){
        return std::make_shared<NewDirObjectEvent>(e);
    }

    Event::Ptr EmptyEvent(Event e){
        return std::make_shared<Event>(e);
    }
}