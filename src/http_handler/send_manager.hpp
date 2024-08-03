#pragma once
#include "http_types.hpp"
#include "public_user.hpp"

namespace http_handler{
    void Send(SessionData rns, status stat, std::string body);
    void SendSuccess(SessionData rns);

    void SendUser(SessionData rns, const PublicUser& user);

    void SendWrongApiFunction(SessionData rns);
    void SendWrongBodyData(SessionData rns);
    void SendLoginTaken(SessionData rns);
    void SendWrongLoginOrPassword(SessionData rns);
    void SendNoSuchUser(SessionData rns);
    void SendUnauthorized(SessionData rns);
    void SendInvalidToken(SessionData rns);
    void SendAdminUnrecognized(SessionData rns);
    void SendTokenToRemovedPerson(SessionData rns);
    void SendCantEnqueue(SessionData rns);
    void SendInTheMatch(SessionData rns);
    void SendCantLogin(SessionData rns);
    void SendPollClosed(SessionData rns, const std::string& description);
    void SendWrongSessionId(SessionData rns);
    void SendAccessDenied(SessionData rns);
    void SendNotYourMove(SessionData rns);
    void SendWrongMove(SessionData rns);
    void SendSessionFinished(SessionData rns);
    void SendWrongUrlParameters(SessionData rns);
}

