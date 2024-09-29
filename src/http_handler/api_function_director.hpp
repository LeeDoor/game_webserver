#pragma once
#include "api_function_builder.hpp"
#include "i_token_manager.hpp"
#include <vector>

namespace http_handler {
    class ApiFunctionDirector {
    public:
        ApiFunctionDirector(token_manager::ITokenManager::Ptr tm);
        
        ApiFunctionExecutor GetPing(ExecutorFunction&& afe);
        ApiFunctionExecutor GetRegister(ExecutorFunction&& afe);
        ApiFunctionExecutor GetLogin(ExecutorFunction&& afe);
        ApiFunctionExecutor GetProfile(ExecutorFunction&& afe);
        
        ApiFunctionExecutor GetEnqueue(ExecutorFunction&& afe);
        ApiFunctionExecutor GetWaitForOpponent(ExecutorFunction&& afe);
        ApiFunctionExecutor GetSessionState(ExecutorFunction&& afe);
        ApiFunctionExecutor GetSessionStateChange(ExecutorFunction&& afe);
        ApiFunctionExecutor GetMove(ExecutorFunction&& afe);
        ApiFunctionExecutor GetGameConsts(ExecutorFunction&& afe);

        ApiFunctionExecutor GetPlayerTokens(ExecutorFunction&& afe);
        ApiFunctionExecutor GetUser(ExecutorFunction&& afe);
        ApiFunctionExecutor GetMatchmakingQueue(ExecutorFunction&& afe);
        ApiFunctionExecutor GetSetState(ExecutorFunction&& afe);

    private:
        ApiFunctionBuilder afb_;
        token_manager::ITokenManager::Ptr tm_;
    };

}