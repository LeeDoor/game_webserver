#pragma once
#include <list>
#include <functional>
#include "event_manager.hpp"
#include "player.hpp"
#include "custom_events.hpp"
#include "i_placeable.hpp"
#include "nlohmann/json.hpp"

namespace game_manager{
    struct State;
    
    class Object : public IPlaceable {
    public:
        using Ptr = std::shared_ptr<Object>;
        using OwnerType = Player::Login;
        using EventsType = EventListWrapper::Vec;

        Object(OwnerType owner, ActorId id);
        virtual ~Object();
        virtual bool operator==(Object::Ptr obj) const;
        virtual EventsType UpdateTick(int move_number) = 0;

        // Although adding serialization logic to the class itself is undesirable, 
        // it is a convenient way to polymorph object serialization.
        virtual void tojson(nlohmann::json& j) const;

        OwnerType owner;
    };
}