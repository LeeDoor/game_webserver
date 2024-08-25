#pragma once
#include "user.hpp"
#include <optional>
#include "session_id.hpp"

namespace session_manager{
    struct SessionData{
        gm::SessionId id;
        um::Uuid winner;
        um::Uuid loser;
    };

    struct PublicSessionData{
        gm::SessionId id;
        std::optional<um::Login> winner;
        std::optional<um::Login> loser;

        bool operator == (const PublicSessionData& other) const{
            return id == other.id && winner == other.winner && loser == other.loser;
        }
    };
}

namespace sm = session_manager;