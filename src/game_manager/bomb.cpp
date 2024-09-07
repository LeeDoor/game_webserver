#include "bomb.hpp"
#include "custom_events.hpp"
#include "state.hpp"

namespace game_manager{
    Bomb::Bomb(OwnerType owner, ActorId id) 
        : Object(owner, id){}
    bool Bomb::operator==(Object::Ptr obj) const {
        Bomb::Ptr d = std::dynamic_pointer_cast<Bomb>(obj);
        return Object::operator==(obj) 
            && d 
            && d->ticks_left == ticks_left;
    }
    void Bomb::tojson(nlohmann::json& j) const {
        Object::tojson(j);
        j["type"] = "bomb";
        j["ticks_left"] = ticks_left;
    } 

    Object::EventsType Bomb::UpdateTick() {
        --ticks_left;
        if(ticks_left)
            return {EmptyEvent({actor_id, BOMB_TICKING})};
        interactor_->Explode(position);
        interactor_->RemoveObject(actor_id);
        return {EmptyEvent({actor_id, BOMB_EXPLODE})};
    }
}