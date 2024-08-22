#pragma once
#include <memory>
#include "player.hpp"
#include "nlohmann/json.hpp"

namespace game_manager{
    struct State;
    
    class Object{
    public:
        using Ptr = std::shared_ptr<Object>;
        using OwnerType = Player::Login;

        Object(OwnerType owner, ActorId id);
        virtual ~Object();
        virtual bool operator==(Object::Ptr obj) const;
        // Although adding serialization logic to the class itself is undesirable, 
        // it is a convenient way to polymorph object serialization.
        virtual void tojson(nlohmann::json& j) const;
        virtual std::string UpdateTick() = 0;
        
        ActorId actor_id;
        OwnerType owner;        
    };
}