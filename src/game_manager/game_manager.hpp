#pragma once
#include "session.hpp"
#include <map>
#include <string>
#include <memory>
#include "i_user_manager.hpp"
#include "i_session_manager.hpp"
#include <mutex>
#include <optional>

namespace game_manager{
    class GameManager{
    public:
        using Ptr = std::shared_ptr<GameManager>;

        GameManager(um::IUserManager::Ptr um, sm::ISessionManager::Ptr sm);

        bool CreateSession(um::Uuid&& player1, um::Uuid&& player2);
        bool HasSession(const SessionId& sid);
        std::optional<SessionId> HasPlayer(const um::Uuid& uuid);
        std::optional<bool> HasPlayer(const um::Uuid& uuid, const SessionId& sessionId);
        State::OptCPtr GetState(const SessionId& sessionId);

        std::optional<bool> ApiResign(const um::Uuid& uuid, const gm::SessionId& sid);

        //ingame api
        std::optional<Session::GameApiStatus> ApiWalk(const um::Uuid& uuid, const SessionId& sid, const Session::WalkData& data); 
        std::optional<Session::GameApiStatus> ApiPlaceBomb(const um::Uuid& uuid, const SessionId& sid, const Session::PlaceBombData& data); 
        
    private:
        bool FinishSession(const SessionId& sid);

        SessionId GenerateSessionId();

        sm::ISessionManager::Ptr sm_;
        um::IUserManager::Ptr dm_;
        std::map<SessionId, Session> sessions_;
    };
}
