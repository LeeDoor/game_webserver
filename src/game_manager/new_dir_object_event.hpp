#pragma once
#include "new_object_event.hpp"
#include "direction.hpp"

namespace game_manager{
    struct NewDirObjectEvent : public NewObjectEvent{
        using Ptr = std::shared_ptr<NewDirObjectEvent>;

        NewDirObjectEvent(NewObjectEvent noe, Direction direction)
            :NewObjectEvent(noe), direction(direction){}
        
        virtual void tojson(nlohmann::json& j) const;
        Direction direction;
    };
}