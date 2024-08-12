#include "object.hpp"
#include "state.hpp"
#include <algorithm>

namespace game_manager{
    Object::Object(OwnerType owner) : owner(owner){}
    Object::~Object() = default;
    bool Object::operator==(Object::Ptr obj) const {
        return obj->owner == owner && 
            obj->posX == posX &&
            obj->posY == posY;
    }

    void Object::Place(Dimention posX, Dimention posY){
        this->posX = posX;
        this->posY = posY;
    }
}