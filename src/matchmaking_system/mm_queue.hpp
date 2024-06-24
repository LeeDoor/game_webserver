#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "game_manager.hpp"
#include "user_data_types.hpp"

namespace matchmaking_system {
    class MMQueue{
    public:
        MMQueue(game_manager::GameManager::Ptr gm);
        using Ptr = std::shared_ptr<MMQueue>;
        
        //adds player to queue
        //returns: true - success; false - player already in queue
        bool EnqueuePlayer(const dm::Uuid& uuid);
        //dequeues player from queue
        //returns: true - success; false - no player in queue
        bool DequeuePlayer(const dm::Uuid& uuid);

        // debug method
        const std::vector<dm::Uuid>& GetQueue() const;
    private:
        // function should be called after each enqueue. checks players to create new session
        void QueueUpdate();

        std::vector<dm::Uuid> queue_;
        game_manager::GameManager::Ptr gm_;
        std::mutex queue_update_mutex_;
    };
}