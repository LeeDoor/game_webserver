#pragma once
#include "response_builder.hpp"
#include "i_serializer.hpp"
#include "token.hpp"
#include "api_function_executor.hpp"
#include "user_data.hpp"
#include "public_user_data.hpp"

namespace http_handler{
    class SendManager{
    public:
        SendManager(serializer::ISerializer::Ptr ser);

        void Send(SessionData rns, status stat, std::string body) const;

        void SendSuccess(SessionData rns) const;
        void SendToken(SessionData rns, token_manager::Token& token) const;
        void SendUserData(SessionData rns, const PublicUserData& user_data) const;
        void SendHiddenUserData(SessionData rns, const dm::UserData& user_data) const;
        void SendSessionId(SessionData rns, const std::string& session_id) const;
        void SendGameState(SessionData rns, const gm::State& state) const;

        void SendWrongApiFunction(SessionData rns) const;
        void SendWrongBodyData(SessionData rns) const;
        void SendLoginTaken(SessionData rns) const;
        void SendWrongLoginOrPassword(SessionData rns) const;
        void SendNoSuchUser(SessionData rns) const;
        void SendUnauthorized(SessionData rns) const;
        void SendInvalidToken(SessionData rns) const;
        void SendAdminUnrecognized(SessionData rns) const;
        void SendTokenToRemovedPerson(SessionData rns) const;
        void SendCantEnqueue(SessionData rns) const;
        void SendInTheMatch(SessionData rns) const;
        void SendCantLogin(SessionData rns) const;
        void SendPollClosed(SessionData rns, const std::string& description) const;
        void SendWrongSessionId(SessionData rns) const;
        void SendAccessDenied(SessionData rns) const;
        void SendNotYourMove(SessionData rns) const;
        void SendWrongMove(SessionData rns) const;

        void SendWrongUrlParameters(SessionData rns) const;

        void HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, SessionData rns) const;
        void SendWrongMethod(const ApiFunctionExecutor& executor, SessionData rns) const;
        void SendUndefinedError(const ApiFunctionExecutor& executor, SessionData rns) const;

    private:
        serializer::ISerializer::Ptr serializer_;
    };
}

