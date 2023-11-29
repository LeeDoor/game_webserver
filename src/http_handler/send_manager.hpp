#pragma once
#include "response_builder.hpp"
#include "i_serializer.hpp"
#include "token.hpp"
#include "api_function_executor.hpp"

namespace http_handler{
    class SendManager{
    public:
        SendManager(serializer::ISerializer::Ptr ser);

        void SendSuccess(RequestNSender rns);
        void SendToken(RequestNSender rns, token_manager::Token& token);
        
        void SendWrongApiFunction(RequestNSender rns);
        void SendWrongBodyData(RequestNSender rns);
        void SendLoginTaken(RequestNSender rns);
        void SendWrongLoginOrPassword(RequestNSender rns);
        void SendNoSuchUser(RequestNSender rns);

        void HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, RequestNSender rns);
        void SendWrongMethod(const ApiFunctionExecutor& executor, RequestNSender rns);
        void SendUndefinedError(const ApiFunctionExecutor& executor, RequestNSender rns);

    private:
        serializer::ISerializer::Ptr serializer_;
    };
}

