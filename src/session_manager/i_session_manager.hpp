#pragma once
#include "session_data.hpp"

namespace session_manager{
    class ISessionManager{
    public:
        virtual bool AddLine(SessionData&& sd) = 0;
        virtual std::optional<SessionData> GetLine(const gm::SessionId& sid) = 0;
    };

}