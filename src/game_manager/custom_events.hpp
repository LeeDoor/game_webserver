#pragma once
#include "position_event.hpp"
#include "new_object_event.hpp"
#include "new_dir_object_event.hpp"
#include "interaction_event.hpp"
namespace game_manager{
    PositionEvent::Ptr WalkEvent(PositionEvent e);
    NewObjectEvent::Ptr BombEvent(NewObjectEvent e);
    NewDirObjectEvent::Ptr GunEvent(NewDirObjectEvent e);
    NewDirObjectEvent::Ptr BulletEvent(NewDirObjectEvent e);
    Event::Ptr EmptyEvent(Event e);
}