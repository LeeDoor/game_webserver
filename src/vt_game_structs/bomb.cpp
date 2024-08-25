#include "bomb.hpp"
#include "custom_events.hpp"

namespace game_manager{
    Bomb::Bomb(OwnerType owner, ActorId id) 
        : Object(owner, id){}
    Bomb::Bomb(OwnerType owner, ActorId id, Methods&& methods) 
        : explode_(std::move(methods.explode)), Object(owner, id, std::move(methods)) {}
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

    Object::EventsType Bomb::UpdateTick(int move_number) {
        --ticks_left;
        if(ticks_left)
            return {EmptyEvent({move_number, actor_id, BOMB_TICKING})};
        explode_(position);
        destroy_(actor_id);
        return {EmptyEvent({move_number, actor_id, BOMB_EXPLODE})};
    }
}