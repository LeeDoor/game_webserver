#include "object.hpp"
#include "type_serializer.hpp"
namespace game_manager{
    Object::Object(OwnerType owner, ActorId id)
        : IPlaceable(id), owner(owner){}
    Object::~Object() = default;
    bool Object::operator==(Object::Ptr obj) const {
        return IPlaceable::operator==(*obj) &&
            obj->owner == owner;
    }

    void Object::tojson(nlohmann::json& j) const {
        j["type"] = "object";
        j["owner"] = owner;
        j["actor_id"] = actor_id;
        j["position"] = position;
    }
}