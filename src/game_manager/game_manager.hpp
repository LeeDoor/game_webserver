#pragma once
#include "session.hpp"
#include <map>
#include <string>
#include <memory>

namespace game_manager{
    class GameManager{
    public:
        using SessionId = std::string;
        using Ptr = std::shared_ptr<GameManager>;
        
        bool CreateSession(const dm::Uuid& player1, const dm::Uuid& player2);

        //ingame api
        bool Ping(const dm::Uuid& player_id, const SessionId& session_id); // placeholder
    private:
        SessionId GenerateSessionId();

        std::map<SessionId, Session> sessions_;
    };
}