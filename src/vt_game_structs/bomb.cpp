#include "bomb.hpp"

namespace game_manager{
    Bomb::Bomb(OwnerType owner, ActorId id) 
        : ObjectPlaced(owner, id){}
    Bomb::Bomb(OwnerType owner, ActorId id, Methods&& methods) 
        : explode_(std::move(methods.explode)), ObjectPlaced(owner, id, std::move(methods)) {}
    bool Bomb::operator==(Object::Ptr obj) const {
        Bomb::Ptr d = std::dynamic_pointer_cast<Bomb>(obj);
        return ObjectPlaced::operator==(obj) 
            && d 
            && d->ticks_left == ticks_left;
    }
    void Bomb::tojson(nlohmann::json& j) const {
        ObjectPlaced::tojson(j);
        j["type"] = "bomb";
        j["ticks_left"] = ticks_left;
    }

    Object::EventsType Bomb::UpdateTick(int move_number) {
        --ticks_left;
        if(ticks_left)
            return {CreateEvent(move_number, BOMB_TICKING)};
        explode_(posX, posY);
        destroy_(actor_id);
        return {CreateEvent(move_number, BOMB_EXPLODE)};
    }
}