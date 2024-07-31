#pragma once
#include "user.hpp"
#include <optional>
#include "../game_manager/session.hpp"

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
    };
}

namespace sm = session_manager;