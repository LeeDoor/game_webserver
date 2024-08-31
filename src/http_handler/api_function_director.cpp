#include "api_function_director.hpp"   

namespace http_handler{
    ApiFunctionDirector::ApiFunctionDirector(token_manager::ITokenManager::Ptr tm)
        : tm_(tm){}

    ApiFunctionExecutor ApiFunctionDirector::GetPing(ExecutorFunction&& afe){
        return afb_.GetHead().ExecFunc(std::move(afe)).GetProduct();
    }
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
    ApiFunctionExecutor ApiFunctionDirector::GetMove(ExecutorFunction&& afe) {
        return afb_.NeedAuthor(tm_).Post().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetResign(ExecutorFunction&& afe){
        return afb_.NeedAuthor(tm_).Post().ExecFunc(std::move(afe)).GetProduct();
    }
    
    ApiFunctionExecutor ApiFunctionDirector::GetPlayerTokens(ExecutorFunction&& afe){
        return afb_.ForDebug().GetHead().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetUser(ExecutorFunction&& afe){
        return afb_.ForDebug().GetHead().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetMatchmakingQueue(ExecutorFunction&& afe){
        return afb_.ForDebug().GetHead().ExecFunc(std::move(afe)).GetProduct();
    }
    ApiFunctionExecutor ApiFunctionDirector::GetSetState(ExecutorFunction&& afe){
        return afb_.ForDebug().Post().ExecFunc(std::move(afe)).GetProduct();
    }
}