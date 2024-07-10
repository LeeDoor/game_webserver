#pragma once
#include "i_queue_manager.hpp"
#include <sw/redis++/redis++.h>

namespace matchmaking_system {
    class QueueManagerRedis : public IQueueManager{
    public:
        using RedisPtr = std::shared_ptr<sw::redis::Redis>;
        QueueManagerRedis(std::string&& redis_data_name, RedisPtr redis);

        bool EnqueuePlayer(const dm::Uuid& uuid) override;
        bool DequeuePlayer(const dm::Uuid& uuid) override;
        std::optional<std::string> PopPlayer() override;
        long long GetLength() override;
        std::vector<dm::Uuid> GetQueue(int start = 0, int end = -1) override;

    private:
        std::string redis_data_name_;
        RedisPtr redis_;
    };
}