#include "bomb.hpp"
#include "state.hpp"

namespace game_manager{
    Bomb::Bomb(OwnerType owner, ActorId id) 
        : Object(owner, id){}
    Bomb::Bomb(OwnerType owner, ActorId id, ExplodeFunc&& explode) 
        : Object(owner, id), explode_(std::move(explode)){}
    bool Bomb::operator==(Object::Ptr obj) const {
        Bomb::Ptr d = dynamic_pointer_cast<Bomb>(obj);
        return Object::operator==(obj) && d && id == obj->id && d->ticks_left == ticks_left;
    }

    std::pair<std::string, bool> Bomb::UpdateTick() {
        --ticks_left;
        if(ticks_left)
            return {BOMB_TICKING, false};
        explode_(posX, posY);
        return {BOMB_EXPLODE, true};
    }
}