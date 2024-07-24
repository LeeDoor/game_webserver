#include "api_function_director.hpp"   

namespace http_handler{
    ApiFunctionDirector::ApiFunctionDirector(serializer::ISerializer::Ptr serializer, token_manager::ITokenManager::Ptr tm)
        : afb_(serializer), tm_(tm){}

    ApiFunctionExecutor ApiFunctionDirector::GetRegister(ExecutorFunction&& afe){
        return afb_.Post().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetLogin(ExecutorFunction&& afe){
        return afb_.Post().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetProfile(ExecutorFunction&& afe){
        return afb_.NeedAuthor(tm_).GetHead().ExecFunc(std::move(afe)).GetProduct();
    }

    ApiFunctionExecutor ApiFunctionDirector::GetEnqueue(ExecutorFunction&& afe){
        return afb_.NeedAuthor(tm_).Post().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetWaitForOpponent(ExecutorFunction&& afe){
        return afb_.NeedAuthor(tm_).GetHead().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetSessionState(ExecutorFunction&& afe) {
        return afb_.NeedAuthor(tm_).GetHead().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetSessionStateChange(ExecutorFunction&& afe) {
        return afb_.NeedAuthor(tm_).GetHead().ExecFunc(std::move(afe)).GetProduct();
    }
    
    ApiFunctionExecutor ApiFunctionDirector::GetPlayerTokens(ExecutorFunction&& afe){
        return afb_.ForDebug().GetHead().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetUserData(ExecutorFunction&& afe){
        return afb_.ForDebug().GetHead().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetMatchmakingQueue(ExecutorFunction&& afe){
        return afb_.ForDebug().GetHead().ExecFunc(std::move(afe)).GetProduct();
    }
}