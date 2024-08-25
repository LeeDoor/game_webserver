#include "bullet.hpp"

namespace game_manager{
    Bullet::Bullet(OwnerType owner, ActorId actor_id) 
        : DirectedObject(owner, actor_id){}
    Bullet::Bullet(OwnerType owner, ActorId actor_id, Methods&& methods) 
        : DirectedObject(owner, actor_id, std::move(methods)){}

    bool Bullet::operator==(Object::Ptr obj) const {
        Bullet::Ptr d = std::dynamic_pointer_cast<Bullet>(obj);
        return DirectedObject::operator==(obj) && d;
    }
    void Bullet::tojson(nlohmann::json& j) const {
        DirectedObject::tojson(j);
        j["type"] = "bullet";
    }

    Object::EventsType Bullet::UpdateTick(int move_number) {
        int mX = 0, mY = 0; // x and y modifiers.
        switch(direction){
        case Direction::Up:
            mY = -1;
            break;
        case Direction::Left:
            mX = 1;
            break;
        case Direction::Down:
            mY = 1;
            break;
        case Direction::Right:
            mX = -1;
            break;
        }
        position.x += mX;
        position.y += mY;


    }
}