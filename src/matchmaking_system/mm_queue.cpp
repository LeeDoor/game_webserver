#include "mm_queue.hpp"
#include <iostream>

namespace matchmaking_system{
    
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
            std::cout << queue_.back() << ' ';
            queue_.pop_back();
            std::cout << "is fighting " << queue_.back() << std::endl;
            queue_.pop_back();
        }
    }
}