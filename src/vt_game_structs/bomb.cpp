#include "bomb.hpp"

namespace game_manager{
    Bomb::Bomb(OwnerType owner, ActorId id) 
        : ObjectPlaced(owner, id){}
    Bomb::Bomb(OwnerType owner, ActorId id, Methods&& methods) 
        : ObjectPlaced(owner, id), methods_(std::move(methods)){}
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

    std::string Bomb::UpdateTick() {
        --ticks_left;
        if(ticks_left)
            return BOMB_TICKING;
        methods_.explode(posX, posY);
        methods_.destroy(actor_id);
        return BOMB_EXPLODE;
    }
}