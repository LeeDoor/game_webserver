#pragma once
#include "http_types.hpp"
#include "user.hpp"
namespace http_handler{
    void SendHiddenUser(SessionData rns, const um::User& user);
}