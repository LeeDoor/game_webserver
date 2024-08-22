#pragma once
#include <memory>
#include "object_placed.hpp"

namespace game_manager{
    class ObjectDirected : public ObjectPlaced{
    public:
        using Ptr = std::shared_ptr<ObjectDirected>;

        ObjectDirected(OwnerType owner, ActorId id);
        virtual ~ObjectDirected();
        virtual bool operator==(Object::Ptr obj) const;
        virtual void tojson(nlohmann::json& j) const;

        void Place(Dimention posX, Dimention posY, Direction dir);
        
        Direction dir;
    };
}