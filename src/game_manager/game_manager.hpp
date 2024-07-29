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
        bool HasSession(const SessionId& sid);
        bool HasPlayer(const dm::Uuid& uuid);
        bool HasPlayer(const dm::Uuid& uuid, const SessionId& sessionId);
        State::OptCPtr GetState(const SessionId& sessionId);

        bool ApiResign(const dm::Uuid& uuid, const gm::SessionId& sid);

        //ingame api
        Session::GameApiStatus ApiWalk(const dm::Uuid& uuid, const SessionId& sid, const Session::WalkData& data); 
        
    private:
        SessionId GenerateSessionId();

        dm::IUserDataManager::Ptr udm_;
        
        std::map<SessionId, Session> sessions_;
    };
}
