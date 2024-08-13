#pragma once
#include "http_types.hpp"
#include "../game_manager/session.hpp"
namespace http_handler{
    void SendSessionId(SessionData rns, const gm::SessionId& session_id);
    void SendInTheMatch(SessionData rns, const gm::SessionId& sid);
    void SendGameState(SessionData rns, const gm::State& state);
    void SendEvents(SessionData rns, const gm::EventListWrapper& events, int from_move);
}