#pragma once
#include <memory>
#include "direction.hpp"
#include "object.hpp"

namespace game_manager{
    class DirectedObject : public Object{
    public:
        using Ptr = std::shared_ptr<DirectedObject>;

        DirectedObject(OwnerType owner, ActorId id) : Object(owner, id){}
        void Place(Position position, Direction direction) {
            Object::Place(position);
            this->direction = direction;
        }
        virtual bool operator==(Object::Ptr obj) const {
            auto ptr = dynamic_pointer_cast<DirectedObject>(obj);
            return Object::operator==(obj) && ptr &&
                ptr->direction == direction;
        }
        void tojson(nlohmann::json& j) const;
        
        Direction direction;
    };

}