#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "game_manager.hpp"

namespace matchmaking_system {
    class MMQueue{
        using Uuid = std::string;
    public:
        MMQueue(game_manager::GameManager::Ptr gm);
        using Ptr = std::shared_ptr<MMQueue>;

        bool EnqueuePlayer(const Uuid& uuid);
        bool DequeuePlayer(const Uuid& uuid);

    private:
        void QueueUpdate();

        std::vector<Uuid> queue_;
        game_manager::GameManager::Ptr gm_;
        std::mutex queue_update_mutex_;
    };
}