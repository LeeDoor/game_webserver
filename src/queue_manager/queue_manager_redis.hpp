#pragma once
#include "i_queue_manager.hpp"
#include <sw/redis++/redis++.h>

namespace game_manager {
    class QueueManagerRedis : public IQueueManager{
    public:
        using RedisPtr = std::shared_ptr<sw::redis::Redis>;
        QueueManagerRedis(std::string&& redis_data_name, std::string&& redis_set_name, RedisPtr redis);

        bool EnqueuePlayer(const um::Uuid& uuid) override;
        bool DequeuePlayer(const um::Uuid& uuid) override;
        std::optional<std::string> PopPlayer() override;
        long long GetLength() override;
        std::vector<um::Uuid> GetQueue(int start = 0, int end = -1) override;
        void OnEnqueueSubscribe(IObserver::Ptr observer) override;
    private:
        void Notify(EventType type) override;

        std::string redis_queue_name_;
        std::string redis_set_name_;
        RedisPtr redis_;
        std::vector<IObserver::Ptr> observers_;
    };
}