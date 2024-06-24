#include "game_manager.hpp"
#include "session_id_generator.hpp"
namespace game_manager{
    bool GameManager::CreateSession(const dm::Uuid& player1, const dm::Uuid& player2){
        SessionId si = GenerateSessionId();
        sessions_[si]; // create new session
        return sessions_.contains(si);
    }

    //ingame api
    bool GameManager::Ping(const dm::Uuid& player_id, const SessionId& session_id){
        if (sessions_.contains(session_id)){
            return sessions_[session_id].Ping(player_id);
        }
        return false;
    }

    GameManager::SessionId GameManager::GenerateSessionId(){
        SessionIdGenerator sig;
        return sig.Generate();
    }
}

