#include "bullet.hpp"

namespace game_manager{
    Bullet::Bullet(OwnerType owner, ActorId actor_id) 
        : ObjectDirected(owner, actor_id){}
    Bullet::Bullet(OwnerType owner, ActorId actor_id, Methods&& methods) 
        : ObjectDirected(owner, actor_id), methods_(std::move(methods)){}

    bool Bullet::operator==(Object::Ptr obj) const {
        Bullet::Ptr d = std::dynamic_pointer_cast<Bullet>(obj);
        return ObjectDirected::operator==(obj) && d;
    }
    void Bullet::tojson(nlohmann::json& j) const {
        ObjectDirected::tojson(j);
        j["type"] = "bullet";
    }

    std::string Bullet::UpdateTick() {
        
    }
}