#pragma once
#include <string>

namespace game_manager{
    class Session{
    public:
        void Ping(const std::string& player_id);
    };
}   