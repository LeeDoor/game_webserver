#pragma once
#include <string>
#include "user_data.hpp"
#include "state.hpp"

namespace game_manager{
    using SessionId = std::string;
    class Session{
    public:
        Session(dm::Uuid&& player1, const dm::Login& login1, dm::Uuid&& player2, const dm::Login& login2);

        bool HasPlayer(const dm::Uuid& uuid);

        State::Ptr GetState();
        
        bool Ping(const dm::Uuid& player_id);

    private:
        State::Ptr state_;

        dm::Uuid player1_;
        dm::Uuid player2_;
    };
}   
namespace gm = game_manager;