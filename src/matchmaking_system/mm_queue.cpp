#include "mm_queue.hpp"
#include <iostream>

namespace matchmaking_system{
    MMQueue::MMQueue(game_manager::GameManager::Ptr gm) : gm_(gm){}
    
    bool MMQueue::EnqueuePlayer(const Uuid& uuid) {
        queue_.emplace_back(uuid);

        QueueUpdate();
        return true;
    }
    bool MMQueue::DequeuePlayer(const Uuid& uuid) {
        queue_.erase(std::remove(queue_.begin(), queue_.end(), uuid), queue_.end());
        return true;
    }

    void MMQueue::QueueUpdate() {
        if (queue_.size() >= 2){
            queue_update_mutex_.lock();

            Uuid p1 = queue_.back();
            queue_.pop_back();
            Uuid p2 = queue_.back();
            queue_.pop_back();
            gm_->CreateSession(p1, p2);

            queue_update_mutex_.unlock();
        }
    }
}