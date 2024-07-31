#pragma once
#include "session_data.hpp"

namespace session_manager{
    class ISessionManager{
    public:
        using Ptr = std::shared_ptr<ISessionManager>;
        virtual bool AddLine(SessionData&& sd) = 0;
        virtual std::optional<PublicSessionData> GetPublicLine(const gm::SessionId& sid) = 0;
    };
}

namespace sm = session_manager;