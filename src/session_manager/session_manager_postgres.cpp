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
            trans.exec_params0("INSERT INTO sessions VALUES ($1, $2, $3);", sd.id, sd.winner, sd.loser);
            trans.commit();
            return true;
        }
        catch(std::exception& ex){
            spdlog::error("database AddLine function terminated with {}", ex.what());
        }
        return false;
    }
    std::optional<PublicSessionData> SessionManagerPostgres::GetPublicLine(const gm::SessionId& sid) {
        cp::ConnectionPool::ConnectionWrapper cw = pool_->GetConnection();
        pqxx::read_transaction trans(*cw);
        try {
            pqxx::row res = 
                trans.exec_params1(
                    "SELECT s.id as id, u1.login as winner, u2.login as loser " 
                    "FROM sessions s "
                    "LEFT JOIN users u1 ON s.winner = u1.id "
                    "LEFT JOIN users u2 ON s.loser = u2.id "
                    "WHERE s.id=$1;", sid);
            PublicSessionData sd = PublicSessionDataFromRes(res);
            return sd;
        } 
        catch (const std::exception& ex) {
            spdlog::error("database GetPublicLine function terminated with {}", ex.what());
            return std::nullopt;
        }
    }
}