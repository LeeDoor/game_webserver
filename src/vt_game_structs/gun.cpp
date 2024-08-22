#include "gun.hpp"

namespace game_manager{
    Gun::Gun(OwnerType owner, ActorId actor_id) 
        : ObjectDirected(owner, actor_id){}
    Gun::Gun(OwnerType owner, ActorId actor_id, Methods&& methods) 
        : ObjectDirected(owner, actor_id), methods_(std::move(methods)){}

    bool Gun::operator==(Object::Ptr obj) const {
        Gun::Ptr d = std::dynamic_pointer_cast<Gun>(obj);
        return ObjectDirected::operator==(obj) && d && 
            ticks_to_shot == d->ticks_to_shot &&
            shots_left == d->shots_left;
    }
    void Gun::tojson(nlohmann::json& j) const {
        ObjectDirected::tojson(j);
        j["type"] = "gun";
        j["ticks_to_shot"] = ticks_to_shot;
        j["shots_left"] = shots_left;
    }

    std::string Gun::UpdateTick() {
    }
}