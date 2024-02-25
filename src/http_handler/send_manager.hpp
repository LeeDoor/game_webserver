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

        void SendSuccess(RequestNSender rns);
        void SendToken(RequestNSender rns, token_manager::Token& token);
        void SendUserData(RequestNSender rns, const PublicUserData& user_data);
        
        void SendWrongApiFunction(RequestNSender rns);
        void SendWrongBodyData(RequestNSender rns);
        void SendLoginTaken(RequestNSender rns);
        void SendWrongLoginOrPassword(RequestNSender rns);
        void SendNoSuchUser(RequestNSender rns);
        void SendUnauthorized(RequestNSender rns);
        void SendInvalidToken(RequestNSender rns);
        void SendTokenToRemovedPerson(RequestNSender rns);
        void SendCantEnqueue(RequestNSender rns);

        void HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, RequestNSender rns);
        void SendWrongMethod(const ApiFunctionExecutor& executor, RequestNSender rns);
        void SendUndefinedError(const ApiFunctionExecutor& executor, RequestNSender rns);

    private:
        serializer::ISerializer::Ptr serializer_;
    };
}

