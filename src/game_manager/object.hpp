#pragma once
#include <memory>
#include <list>
#include "player.hpp"
#include "event_manager.hpp"
#include "nlohmann/json.hpp"

namespace game_manager{
    struct State;
    
    class Object{
    public:
        using Ptr = std::shared_ptr<Object>;
        using OwnerType = Player::Login;
        using EventsType = EventListWrapper::Vec;
        using DestroyFunc = std::function<void(ActorId)>;

        struct Methods{
            DestroyFunc destroy;
        };

        Object(OwnerType owner, ActorId id);
        Object(OwnerType owner, ActorId id, Methods&& methods);
        virtual ~Object();
        virtual bool operator==(Object::Ptr obj) const;
        // Although adding serialization logic to the class itself is undesirable, 
        // it is a convenient way to polymorph object serialization.
        virtual void tojson(nlohmann::json& j) const;
        virtual EventsType UpdateTick(int move_number) = 0;
        
        ActorId actor_id;
        OwnerType owner;        

    protected:
        DestroyFunc destroy_;

        Event CreateEvent(int move_number, std::string event_type, VariantEventData&& data = EmptyData{});
    };
}