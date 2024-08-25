#pragma once
#include <memory>
#include "user.hpp"
#include "i_placeable.hpp"

namespace game_manager{
    struct Player : public IPlaceable{
        using Ptr = std::shared_ptr<Player>;
        using Login = um::Login;

        Player() {}
        Player(Position position, ActorId actor_id, Login login) 
            : IPlaceable(actor_id), login(login){
            Place(position);
        }
        bool operator == (const Player& other) const = default;

        Login login;
    };
}