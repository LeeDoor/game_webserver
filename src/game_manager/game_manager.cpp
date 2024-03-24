#include "game_manager.hpp"

namespace game_manager{
    bool GameManager::CreateSession(const std::string& player1, const std::string& player2){
        SessionId si = GenerateSessionId();
        sessions_[si]; // create new session
        return sessions_.contains(si);
    }

    //ingame api
    bool GameManager::Ping(const std::string& player_id, const SessionId& session_id){
        if (sessions_.contains(session_id)){
            return sessions_[session_id].Ping(player_id);
        }
        return false;
    }

    GameManager::SessionId GameManager::GenerateSessionId(){
        return "123123123";
    }
}

