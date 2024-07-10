#include "matchmaking_ballancer.hpp"
#include "spdlog/spdlog.h"

namespace matchmaking_system{

    MatchmakingBallancer::MatchmakingBallancer(IQueueManager::Ptr iqm,  game_manager::GameManager::Ptr gm) 
        : iqm_(iqm), gm_(gm){}
    bool MatchmakingBallancer::Ballance() {
        long long queue_length = iqm_->GetLength();
        if (queue_length < 2)
            return true;
        std::optional<std::string> first, second;
        first = iqm_->PopPlayer();
        second = iqm_->PopPlayer();
        if (!first.has_value() || !second.has_value()){
            spdlog::error("MatchmakingBallancer::Ballance popping 2 players error");
            return false;  
        }
        gm_->CreateSession(*first, *second);
        return true;
    }
}