#pragma once
#include "user.hpp"
#include "session.hpp"

namespace session_manager{
    struct SessionData{
        gm::SessionId id;
        um::Uuid player1;
        um::Uuid player2;
        bool winner;
    };
}