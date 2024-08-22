#include "object_placed.hpp"

namespace game_manager{
    ObjectPlaced::ObjectPlaced(OwnerType owner, ActorId id) 
        : Object(owner, id){}
    ObjectPlaced::ObjectPlaced(OwnerType owner, ActorId id, Methods&& methods)
        :Object(owner, id, std::move(methods)){}
    ObjectPlaced::~ObjectPlaced() = default;
    bool ObjectPlaced::operator==(Object::Ptr obj) const {
        ObjectPlaced::Ptr d = std::dynamic_pointer_cast<ObjectPlaced>(obj);
        return Object::operator==(obj) && 
            d && 
            posX == d->posX && 
            posY == d->posY;
        
    }
    void ObjectPlaced::tojson(nlohmann::json& j) const {
        Object::tojson(j);
        j["posX"] = posX;
        j["posY"] = posY;
    }

    void ObjectPlaced::Place(Dimention posX, Dimention posY){
        this->posX = posX;
        this->posY = posY;
    }
}