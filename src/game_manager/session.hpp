#pragma once
#include <string>
#include "user_data.hpp"

namespace game_manager{
    using SessionId = std::string;
    class Session{
    public:
        Session(dm::Uuid&& player1, dm::Uuid&& player2);

        bool HasPlayer(const dm::Uuid& uuid);

        bool Ping(const dm::Uuid& player_id);

    private:
        dm::Uuid player1_;
        dm::Uuid player2_;
    };
}   