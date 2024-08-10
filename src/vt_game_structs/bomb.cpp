#include "bomb.hpp"
#include "state.hpp"

namespace game_manager{
    Bomb::Bomb(OwnerType owner, StateType state) 
        : Object(owner, state){}
    Bomb::Bomb(OwnerType owner) 
        : Object(owner){}
    bool Bomb::operator==(Object::Ptr obj) const {
        Bomb::Ptr d = dynamic_pointer_cast<Bomb>(obj);
        return Object::operator==(obj) && d && d->ticks_left == ticks_left;
    }

    bool Bomb::UpdateTick() {
        --ticks_left;
        if(ticks_left)
            return true;

        return false;
    }
}