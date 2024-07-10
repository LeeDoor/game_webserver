#pragma once
#include "i_queue_manager.hpp"
#include "game_manager.hpp"

namespace matchmaking_system {
    class MatchmakingBallancer{
    public:
        using Ptr = std::shared_ptr<MatchmakingBallancer>;
        MatchmakingBallancer(IQueueManager::Ptr iqm, game_manager::GameManager::Ptr gm);

        bool Ballance();
    private:
        IQueueManager::Ptr iqm_;
        game_manager::GameManager::Ptr gm_;
    };
}