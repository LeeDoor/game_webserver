#pragma once
#include <memory>
#include "position.hpp"
#include "i_actor.hpp"
#include "i_state_related.hpp"

namespace game_manager{
    class IPlaceable : public IStateRelated, public IActor {
    public:
        using Ptr = std::shared_ptr<IPlaceable>;
        
        void Place(Position position) {
            this->position = position;
        }
        virtual bool operator==(const IPlaceable& other) const {
            return IActor::operator==(other) && 
                position == other.position;
        }

        Position position;

    protected:
        IPlaceable() {}
        IPlaceable(ActorId actor_id)
            :IActor(actor_id){}
    };

}