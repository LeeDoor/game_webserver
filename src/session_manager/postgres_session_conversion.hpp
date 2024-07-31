#include "session_data.hpp"
#include <pqxx/pqxx>

namespace session_manager {
    static SessionData SessionDataFromRes(const pqxx::row& row) {
        SessionData instance;
        instance.id = row.at("id").as<gm::SessionId>();
        instance.winner = row.at("winner").as<um::Uuid>();
        instance.loser = row.at("loser").as<um::Uuid>();
        return instance;
    }
    static PublicSessionData PublicSessionDataFromRes(const pqxx::row& row) {
        PublicSessionData instance;
        instance.id = row.at("id").as<gm::SessionId>();
        if(row.at("winner").is_null())
            instance.winner = std::nullopt;
        else instance.winner = row.at("winner").as<um::Login>();
        if(row.at("loser").is_null())
            instance.loser = std::nullopt;
        else instance.loser = row.at("loser").as<um::Login>();
        return instance;
    }
}