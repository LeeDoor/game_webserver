#include "object.hpp"
#include "state.hpp"
#include <algorithm>

namespace game_manager{
    Object::Object(OwnerType owner, StateType state) 
        :owner(owner), state_(state){}
    Object::~Object() = default;
    void Object::Place(Dimention posX, Dimention posY){
        this->posX = posX;
        this->posY = posY;
    }
}