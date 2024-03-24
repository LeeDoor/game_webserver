#pragma once
#include <string>

namespace game_manager{
    class Session{
    public:
        bool Ping(const std::string& player_id);
    };
}   