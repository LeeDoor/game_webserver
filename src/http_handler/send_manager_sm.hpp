#pragma once
#include "http_types.hpp"
#include "session_data.hpp"
namespace http_handler{
    void SendFinishedState(SessionData rns, const session_manager::PublicSessionData& data);
}