#include "session_data.hpp"
#include <pqxx/pqxx>

namespace session_manager {
    static SessionData from_result(const pqxx::row& row) {
        SessionData instance;
        instance.id = row.at("id").as<gm::SessionId>();
        instance.player1 = row.at("player1").as<um::Uuid>();
        instance.player2 = row.at("player2").as<um::Uuid>();
        instance.winner = row.at("winner").as<bool>();
        return instance;
    }
}