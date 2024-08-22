#include "bullet.hpp"

namespace game_manager{
    Bullet::Bullet(OwnerType owner, ActorId actor_id) 
        : ObjectDirected(owner, actor_id){}
    Bullet::Bullet(OwnerType owner, ActorId actor_id, Methods&& methods) 
        : get_shootables_(std::move(methods.get_shootables)), ObjectDirected(owner, actor_id, std::move(methods)){}

    bool Bullet::operator==(Object::Ptr obj) const {
        Bullet::Ptr d = std::dynamic_pointer_cast<Bullet>(obj);
        return ObjectDirected::operator==(obj) && d;
    }
    void Bullet::tojson(nlohmann::json& j) const {
        ObjectDirected::tojson(j);
        j["type"] = "bullet";
    }

    Object::EventsType Bullet::UpdateTick(int move_number) {
        int mX = 0, mY = 0; // x and y modifiers.
        switch(dir){
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
        posX += mX;
        posY += mY;

        std::optional<IShootable::Arr> shootables = get_shootables_(shared_from_this());
        if(!shootables) 
            return {CreateEvent(move_number, BULLET_DESTROY)};
        Object::EventsType events = {CreateEvent(move_number, BULLET_FLY)};
        
        for(IShootable::Ptr shootable : *shootables){
            events.splice(events.end(), shootable->GetShot(move_number, shared_from_this()));
        }

        if(shootables->size() > 0) {
            events.push_back(CreateEvent(move_number, BULLET_DESTROY));
        }
        return events;
    }
}