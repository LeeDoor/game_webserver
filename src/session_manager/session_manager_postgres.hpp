#pragma once
#include "i_session_manager.hpp"
#include "postgres_connection_pool.hpp"
namespace session_manager{
    class SessionManagerPostgres : public ISessionManager{
    public:
        SessionManagerPostgres(cp::ConnectionPool::Ptr pool);

        bool AddLine(SessionData&& sd) override;
        std::optional<SessionData> GetLine(const gm::SessionId& sid) override;

    private:
        cp::ConnectionPool::Ptr pool_;
    };
}