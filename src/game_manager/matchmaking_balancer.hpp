#pragma once
#include "i_queue_manager.hpp"
#include "game_manager.hpp"

namespace game_manager {
    class MatchmakingBalancer : public IQueueManager::IObserver{
    public:
        using Ptr = std::shared_ptr<MatchmakingBalancer>;
        MatchmakingBalancer(IQueueManager::Ptr iqm, game_manager::GameManager::Ptr gm);

        bool Ballance();

        void Notify(IQueueManager::EventType event_type) override;
    private:
        IQueueManager::Ptr iqm_;
        game_manager::GameManager::Ptr gm_;
    };
}