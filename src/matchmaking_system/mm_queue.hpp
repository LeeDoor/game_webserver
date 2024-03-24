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
        
        //adds player to queue
        //returns: true - success; false - player already in queue
        bool EnqueuePlayer(const Uuid& uuid);
        //dequeues player from queue
        //returns: true - success; false - no player in queue
        bool DequeuePlayer(const Uuid& uuid);

    private:
        // function should be called after each enqueue. checks players to create new session
        void QueueUpdate();

        std::vector<Uuid> queue_;
        game_manager::GameManager::Ptr gm_;
        std::mutex queue_update_mutex_;
    };
}