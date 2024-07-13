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

        void Send(SessionData rns, status stat, std::string body);

        void SendSuccess(SessionData rns);
        void SendToken(SessionData rns, token_manager::Token& token);
        void SendUserData(SessionData rns, const PublicUserData& user_data);
        void SendHiddenUserData(SessionData rns, const dm::UserData& user_data);
        
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
        void SendCantLogin(SessionData rns);

        void SendWrongUrlParameters(SessionData rns);

        void HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, SessionData rns);
        void SendWrongMethod(const ApiFunctionExecutor& executor, SessionData rns);
        void SendUndefinedError(const ApiFunctionExecutor& executor, SessionData rns);

    private:
        serializer::ISerializer::Ptr serializer_;
    };
}

