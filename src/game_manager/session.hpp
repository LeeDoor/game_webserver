#pragma once
#include <string>
#include "user_data.hpp"

namespace game_manager{
    class Session{
    public:
        bool Ping(const dm::Uuid& player_id);
    };
}   