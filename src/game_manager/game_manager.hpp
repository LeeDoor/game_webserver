#pragma once
#include "session.hpp"
#include <map>
#include <string>
#include <memory>
#include "i_user_data_manager.hpp"

namespace game_manager{
    class GameManager{
    public:
        using Ptr = std::shared_ptr<GameManager>;

        GameManager(dm::IUserDataManager::Ptr udm);

        bool CreateSession(dm::Uuid&& player1, dm::Uuid&& player2);
        bool HasPlayer(const dm::Uuid& uuid);

        //ingame api
        bool Ping(const dm::Uuid& player_id, const SessionId& session_id); // placeholder
    private:
        SessionId GenerateSessionId();

        dm::IUserDataManager::Ptr udm_;
        
        std::map<SessionId, Session> sessions_;
    };
}
