#include "object.hpp"

namespace game_manager{
    Object::Object(OwnerType owner, ActorId id) 
        : Object(owner, id, {}){}
    Object::Object(OwnerType owner, ActorId id, Methods&& methods)
        : owner(owner), actor_id(id), destroy_(std::move(methods.destroy)){}
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

    Event Object::CreateEvent(int move_number, std::string event_type, VariantEventData&& data) {
        return {move_number, actor_id, std::move(event_type), std::move(data)};
    }
}