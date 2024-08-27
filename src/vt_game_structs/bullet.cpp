#include "bullet.hpp"

namespace game_manager{
    Bullet::Bullet(OwnerType owner, ActorId actor_id) 
        : DirectedObject(owner, actor_id){}
    Bullet::Bullet(OwnerType owner, ActorId actor_id, Methods&& methods) 
        : get_collisions_(std::move(methods.get_collisions)), DirectedObject(owner, actor_id, std::move(methods)){}

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
            mX = -1;
            break;
        case Direction::Down:
            mY = 1;
            break;
        case Direction::Right:
            mX = 1;
            break;
        }
        
        position.x += mX;
        position.y += mY;

        std::optional<std::list<IPlaceable::Ptr>> collisions = get_collisions_(shared_from_this());
        if(collisions) {
            if (collisions->size() == 0){
                return {EmptyEvent({move_number, actor_id, BULLET_FLY})};
            }
        }

        destroy_(actor_id);
        return {EmptyEvent({move_number, actor_id, BULLET_DESTROY})};
    }
} 