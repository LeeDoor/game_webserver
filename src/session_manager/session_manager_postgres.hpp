#pragma once
#include "i_session_manager.hpp"

namespace session_manager{
    class SessionManagerPostgres : public ISessionManager{
    public:
        bool AddLine(SessionData&& sd) override;
        std::optional<SessionData> GetLine(const gm::SessionId& sid) override;
    private:
    };
}