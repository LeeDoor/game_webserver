#pragma once
#include "i_queue_manager.hpp"
#include <sw/redis++/redis++.h>

namespace matchmaking_system {
    class QueueManagerRedis : public IQueueManager{
    public:
        using RedisPtr = std::shared_ptr<sw::redis::Redis>;
        QueueManagerRedis(std::string&& redis_data_name, RedisPtr redis);

        //adds player to queue
        //returns: true - success; false - player already in queue
        bool EnqueuePlayer(const dm::Uuid& uuid) override;
        //dequeues player from queue
        //returns: true - success; false - no player in queue
        bool DequeuePlayer(const dm::Uuid& uuid) override;

        // start and end are indices of first and last parsing objects
        // -1 is last, -2 is penultimate etc.
        std::vector<dm::Uuid> GetQueue(int start = 0, int end = -1) override;

    private:
        std::string redis_data_name_;
        RedisPtr redis_;
    };
}