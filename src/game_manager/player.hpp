#pragma once
#include <memory>
#include "user.hpp"
#include "i_placeable.hpp"
#include "event_manager.hpp"
#include "custom_events.hpp"

namespace game_manager{
    class Player : public IPlaceable{
    public:
        using Ptr = std::shared_ptr<Player>;
        using Login = um::Login;

        Player();
        Player(Position position, ActorId actor_id, Login login);
        bool operator == (const Player& other) const;

        EventListWrapper::Vec Die(int move_number);

        Login login;
    private:
        const std::string PLAYER_DEAD = "player_dead";
    };
}