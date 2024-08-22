#include "object_directed.hpp"

namespace game_manager{
    ObjectDirected::ObjectDirected(OwnerType owner, ActorId id) 
        : ObjectPlaced(owner, id){}
    ObjectDirected::ObjectDirected(OwnerType owner, ActorId id, Methods&& methods)
        : ObjectPlaced(owner, id, std::move(methods)){}
    ObjectDirected::~ObjectDirected() = default;
    bool ObjectDirected::operator==(Object::Ptr obj) const {
        ObjectDirected::Ptr d = std::dynamic_pointer_cast<ObjectDirected>(obj);
        return ObjectPlaced::operator==(obj) && 
            d && 
            dir == d->dir;
    }
    void ObjectDirected::tojson(nlohmann::json& j) const {
        ObjectPlaced::tojson(j);
        j["dir"] = dir;
    }

    void ObjectDirected::Place(Dimention posX, Dimention posY, Direction dir){
        ObjectPlaced::Place(posX, posY);
        this->dir = dir;
    }
}