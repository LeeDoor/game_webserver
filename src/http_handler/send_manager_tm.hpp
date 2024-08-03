#pragma once
#include "http_types.hpp"
#include "token.hpp"
namespace http_handler{
    void SendToken(SessionData rns, token_manager::Token& token);
}