#include "directed_object.hpp"
#include "type_serializer.hpp"
namespace game_manager{
    void DirectedObject::tojson(nlohmann::json& j) const {
        Object::tojson(j);
        j["direction"] = direction;
    }
}