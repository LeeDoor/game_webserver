#pragma once
#include <memory>
#include "position.hpp"
#include "actor_id.hpp"

namespace game_manager{
    class IPlaceable {
    public:
        using Ptr = std::shared_ptr<IPlaceable>;
        
        void Place(Position position) {
            this->position = position;
        }
        virtual bool operator==(const IPlaceable& other) const = default;

        Position position;
        ActorId actor_id;

    protected:
        IPlaceable() {}
        IPlaceable(ActorId actor_id)
            :actor_id(actor_id){}
    };

}