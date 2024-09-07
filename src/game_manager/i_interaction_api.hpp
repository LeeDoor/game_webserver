#pragma once
#include "i_state.hpp"
#include "bomb.hpp"
#include "gun.hpp"
#include "bullet.hpp"
#include <list>
#include <optional>

namespace game_manager{
    class IInteractionApi{
    public:
        using Ptr = std::shared_ptr<IInteractionApi>;

        virtual void FinishSession(bool firstWinner) = 0;
        virtual Bomb::Ptr PlaceBombObject(Position position, Object::OwnerType login) = 0;
        virtual Gun::Ptr PlaceGunObject(Position position, Direction direction, Object::OwnerType login) = 0;
        virtual Bullet::Ptr PlaceBulletObject(Position position, Direction direction, Object::OwnerType login) = 0;
        virtual void RemoveObject(ActorId actor_id) = 0;
        virtual void Explode(Position position) = 0;
        virtual std::optional<std::list<IPlaceable::Ptr>> CollisionsOnCell(Bullet::Ptr bullet) = 0;
    };
}