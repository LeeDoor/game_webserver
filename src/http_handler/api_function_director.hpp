#pragma once
#include "api_function_builder.hpp"
#include "i_token_manager.hpp"
#include <vector>

namespace http_handler {
    class ApiFunctionDirector {
    public:
        ApiFunctionDirector(serializer::ISerializer::Ptr serializer_, token_manager::ITokenManager::Ptr tm);

        ApiFunctionExecutor GetRegister(ExecutorFunction&& afe);
        ApiFunctionExecutor GetLogin(ExecutorFunction&& afe);
        ApiFunctionExecutor GetProfile(ExecutorFunction&& afe);
        
        ApiFunctionExecutor GetEnqueue(ExecutorFunction&& afe);
        ApiFunctionExecutor GetWaitForOpponent(ExecutorFunction&& afe);
        ApiFunctionExecutor GetSessionState(ExecutorFunction&& afe);
        ApiFunctionExecutor GetSessionStateChange(ExecutorFunction&& afe);
        ApiFunctionExecutor GetMove(ExecutorFunction&& afe);
        ApiFunctionExecutor GetResign(ExecutorFunction&& afe);

        ApiFunctionExecutor GetPlayerTokens(ExecutorFunction&& afe);
        ApiFunctionExecutor GetUser(ExecutorFunction&& afe);
        ApiFunctionExecutor GetMatchmakingQueue(ExecutorFunction&& afe);

    private:
        ApiFunctionBuilder afb_;
        token_manager::ITokenManager::Ptr tm_;
    };

}