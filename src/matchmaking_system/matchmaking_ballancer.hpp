#pragma once
#include "i_queue_manager.hpp"

namespace matchmaking_system {
    class MatchmakingBallancer{
    public:
        using Ptr = std::shared_ptr<MatchmakingBallancer>;
        MatchmakingBallancer(IQueueManager::Ptr iqm);

        void Ballance();
    private:
        IQueueManager::Ptr iqm_;
    };
}