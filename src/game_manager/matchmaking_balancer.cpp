#include "matchmaking_balancer.hpp"
#include "spdlog/spdlog.h"

namespace game_manager{

    MatchmakingBalancer::MatchmakingBalancer(IQueueManager::Ptr iqm,  game_manager::GameManager::Ptr gm) 
        : iqm_(iqm), gm_(gm){}
    bool MatchmakingBalancer::Ballance() {
        long long queue_length = iqm_->GetLength();
        if (queue_length < 2)
            return true;
        std::optional<std::string> first, second;
        first = iqm_->PopPlayer();
        second = iqm_->PopPlayer();
        if (!first.has_value() || !second.has_value()){
            spdlog::error("MatchmakingBalancer::Ballance popping 2 players error");
            return false;  
        }
        gm_->CreateSession(std::move(*first), std::move(*second));
        return true;
    }
    void MatchmakingBalancer::Notify(IQueueManager::EventType event_type) {
        std::lock_guard<std::mutex> locker(mutex_);
        switch (event_type){
        case IQueueManager::EventType::OnEnqueue:
            Ballance();
            break;
        }
    }
}