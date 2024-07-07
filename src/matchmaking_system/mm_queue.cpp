#include "mm_queue.hpp"
#include <algorithm>

namespace matchmaking_system{
    MMQueue::MMQueue(game_manager::GameManager::Ptr gm) : gm_(gm){}
    
    bool MMQueue::EnqueuePlayer(const dm::Uuid& uuid) {
        if (std::find(queue_.begin(), queue_.end(), uuid) != queue_.end())
            return false;
        queue_.emplace_back(uuid);

        QueueUpdate();
        return true;
    }
    bool MMQueue::DequeuePlayer(const dm::Uuid& uuid) {
        if (std::find(queue_.begin(), queue_.end(), uuid) == queue_.end()){
            return false;
        }
        queue_.erase(std::remove(queue_.begin(), queue_.end(), uuid), queue_.end());
        return true;
    }

    void MMQueue::QueueUpdate() {
        if (queue_.size() >= 2){
            queue_update_mutex_.lock();

            dm::Uuid p1 = queue_.back();
            queue_.pop_back();
            dm::Uuid p2 = queue_.back();
            queue_.pop_back();
            gm_->CreateSession(p1, p2);

            queue_update_mutex_.unlock();
        }
    }

    const std::vector<dm::Uuid>& MMQueue::GetQueue() const {
        return queue_;
    } 
}