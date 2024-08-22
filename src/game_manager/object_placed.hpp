#pragma once
#include <memory>
#include "object.hpp"

namespace game_manager{
    class ObjectPlaced : public Object{
    public:
        using Ptr = std::shared_ptr<ObjectPlaced>;

        ObjectPlaced(OwnerType owner, ActorId id);
        ObjectPlaced(OwnerType owner, ActorId id, Methods&& methods);
        virtual ~ObjectPlaced();
        virtual bool operator==(Object::Ptr obj) const;
        virtual void tojson(nlohmann::json& j) const;

        void Place(Dimention posX, Dimention posY);
        
        Dimention posX, posY;   
    };
}