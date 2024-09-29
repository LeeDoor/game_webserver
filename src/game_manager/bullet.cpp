#include "bullet.hpp"
#include "session.hpp"

namespace game_manager{
    Bullet::Bullet(OwnerType owner, ActorId actor_id) 
        : DirectedObject(owner, actor_id){}
        
    bool Bullet::operator==(Object::Ptr obj) const {
        Bullet::Ptr d = std::dynamic_pointer_cast<Bullet>(obj);
        return DirectedObject::operator==(obj) && d;
    }
    void Bullet::tojson(nlohmann::json& j) const {
        DirectedObject::tojson(j);
        j["type"] = "bullet";
    }

    EventsType Bullet::UpdateTick() {
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

        EventsType events;

        std::optional<std::list<IPlaceable::Ptr>> collisions = interactor_->CollisionsOnCell(shared_from_this());
        if(collisions) {
            if (collisions->size() == 0){
                events.push_back(EmptyEvent({actor_id, BULLET_FLY}));
                return events;
            }

            for(IPlaceable::Ptr coll : *collisions) {
                auto subevents = coll->InteractWithBullet(shared_from_this());
                events.push_back(CreateInteractionEvent({{actor_id, BULLET_INTERACTION}, coll->actor_id, subevents}));
            }
        }

        interactor_->RemoveObject(actor_id);
        events.push_back(EmptyEvent({actor_id, BULLET_DESTROY}));
        return events;
    }

    EventsType Bullet::InteractWithBullet(std::shared_ptr<Bullet> bullet) {
        interactor_->RemoveObject(actor_id);
        return {EmptyEvent({actor_id, BULLET_DESTROY})};
    }
} 