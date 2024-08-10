#include "bomb.hpp"

namespace game_manager{
    Bomb::Bomb(OwnerType owner, StateType state) 
        : Object(owner, state){}

    bool Bomb::UpdateTick() {
        --ticks_left;
        if(ticks_left)
            return true;

        // explode
        return false;
    }
}