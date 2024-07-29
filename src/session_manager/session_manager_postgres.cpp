#include "session_manager_postgres.hpp"
#include "spdlog/spdlog.h"
#include "postgres_session_conversion.hpp"
namespace session_manager{
    SessionManagerPostgres::SessionManagerPostgres(cp::ConnectionPool::Ptr pool)
        :pool_(pool){}
    bool SessionManagerPostgres::AddLine(SessionData&& sd) {
        try{
            cp::ConnectionPool::ConnectionWrapper cw = pool_->GetConnection();
            pqxx::work trans(*cw);
            trans.exec_params0("INSERT INTO sessions VALUES ($1, $2, $3, $4);", sd.id, sd.player1, sd.player2, sd.winner);
            trans.commit();
            return true;
        }
        catch(std::exception& ex){
            spdlog::error("database AddLine function terminated with {}", ex.what());
        }
        return false;
    }
    std::optional<SessionData> SessionManagerPostgres::GetLine(const gm::SessionId& sid) {
        cp::ConnectionPool::ConnectionWrapper cw = pool_->GetConnection();
        pqxx::read_transaction trans(*cw);
        try {
            pqxx::row res = 
                trans.exec_params1("SELECT * FROM sessions WHERE id=$1;", sid);
            SessionData sd = from_result(res);
            return sd;
        } 
        catch (const std::exception& ex) {
            spdlog::error("database GetByUuid function terminated with {}", ex.what());
            return std::nullopt;
        }
    }
}