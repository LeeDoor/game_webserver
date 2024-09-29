#include "gun.hpp"
#include "session.hpp"
#include "const_variables.hpp"

namespace game_manager{
    Gun::Gun(OwnerType owner, ActorId actor_id) 
        :  DirectedObject(owner, actor_id),
        shot_cooldown_(GUN_COOLDOWN),
        shot_amount_(GUN_SHOTS){
            ticks_to_shot = shot_cooldown_; 
            shots_left = shot_amount_;
        }

    bool Gun::operator==(Object::Ptr obj) const {
        Gun::Ptr d = std::dynamic_pointer_cast<Gun>(obj);
        return DirectedObject::operator==(obj) && d && 
            ticks_to_shot == d->ticks_to_shot &&
            shots_left == d->shots_left;
    }
    void Gun::tojson(nlohmann::json& j) const {
        DirectedObject::tojson(j);
        j["type"] = "gun";
        j["ticks_to_shot"] = ticks_to_shot;
        j["shots_left"] = shots_left;
    }

    EventsType Gun::UpdateTick() {
        EventsType events;
        --ticks_to_shot;
        if(!ticks_to_shot) {
            ticks_to_shot = shot_cooldown_;
            ActorId bulletAI = interactor_->PlaceBulletObject(position, direction, owner)->actor_id;
            --shots_left;
            events.push_back(BulletEvent({{{{actor_id, GUN_SHOT}, position},bulletAI},direction}));
            if(!shots_left){
                interactor_->RemoveObject(actor_id);
                events.push_back(EmptyEvent({actor_id, GUN_DESTROY}));
                return events;
            }
            return events;
        }
        return {EmptyEvent({actor_id, GUN_WAITING})};
    }
}