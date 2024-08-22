#pragma once
#include "object_placed.hpp"
#include <functional>

namespace game_manager{
    class Bomb : public ObjectPlaced {
    public:
        using Ptr = std::shared_ptr<Bomb>;

        using ExplodeFunc = std::function<void(Dimention, Dimention)>;
        using DestroyFunc = std::function<void(ActorId)>;

        struct Methods{
            DestroyFunc destroy;
            ExplodeFunc explode;
        };

        Bomb(OwnerType owner, ActorId id);
        Bomb(OwnerType owner, ActorId id, Methods&& methods);
        bool operator==(Object::Ptr obj) const override;
        virtual void tojson(nlohmann::json& j) const;

        std::string UpdateTick() override;

        unsigned ticks_left = 3;

    private:
        Methods methods_;

        const std::string BOMB_TICKING = "bomb_ticking";
        const std::string BOMB_EXPLODE = "bomb_explode";
    };
}