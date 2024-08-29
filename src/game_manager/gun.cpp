#include "gun.hpp"

namespace game_manager{
    Gun::Gun(OwnerType owner, ActorId actor_id) 
        : Gun(owner, actor_id, {}){}
    Gun::Gun(OwnerType owner, ActorId actor_id, Methods&& methods) 
        : 
        shot_cooldown_(3),
        shot_amount_(3),
        shoot_(std::move(methods.shoot)), 
        DirectedObject(owner, actor_id, std::move(methods)){
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

    Object::EventsType Gun::UpdateTick(int move_number) {
        EventsType events;
        --ticks_to_shot;
        if(!ticks_to_shot) {
            ticks_to_shot = shot_cooldown_;
            ActorId bulletAI = shoot_(shared_from_this());
            --shots_left;
            events.push_back(BulletEvent({{{{move_number, actor_id, GUN_SHOT}, position},bulletAI},direction}));
            if(!shots_left){
                destroy_(actor_id);
                events.push_back(EmptyEvent({move_number, actor_id, GUN_DESTROY}));
                return events;
            }
            return events;
        }
        return {EmptyEvent({move_number, actor_id, GUN_WAITING})};
    }
}