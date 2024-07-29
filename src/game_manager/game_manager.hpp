#pragma once
#include "session.hpp"
#include <map>
#include <string>
#include <memory>
#include "i_user_manager.hpp"

namespace game_manager{
    class GameManager{
    public:
        using Ptr = std::shared_ptr<GameManager>;

        GameManager(um::IUserManager::Ptr um);

        bool CreateSession(um::Uuid&& player1, um::Uuid&& player2);
        bool HasSession(const SessionId& sid);
        bool HasPlayer(const um::Uuid& uuid);
        bool HasPlayer(const um::Uuid& uuid, const SessionId& sessionId);
        State::OptCPtr GetState(const SessionId& sessionId);

        bool ApiResign(const um::Uuid& uuid, const gm::SessionId& sid);

        //ingame api
        Session::GameApiStatus ApiWalk(const um::Uuid& uuid, const SessionId& sid, const Session::WalkData& data); 
        
    private:
        SessionId GenerateSessionId();

        um::IUserManager::Ptr dm_;
        
        std::map<SessionId, Session> sessions_;
    };
}
