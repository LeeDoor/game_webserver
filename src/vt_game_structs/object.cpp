#include "object.hpp"

namespace game_manager{
    Object::Object(OwnerType owner, ActorId id) : owner(owner), actor_id(id){}
    Object::~Object() = default;
    bool Object::operator==(Object::Ptr obj) const {
        return obj->owner == owner && 
            obj->actor_id == actor_id;
    }

    void Object::tojson(nlohmann::json& j) const {
        j["type"] = "object";
        j["owner"] = owner;
        j["actor_id"] = actor_id;
    }
}