#include "queue_manager_redis.hpp"

namespace matchmaking_system{
    QueueManagerRedis::QueueManagerRedis(std::string&& redis_data_name, RedisPtr redis)
    :   redis_data_name_(redis_data_name),
        redis_(redis){}

    bool QueueManagerRedis::EnqueuePlayer(const dm::Uuid& uuid) {
        long long res;
        res = redis_->lpush(redis_data_name_, uuid);
        return res == 1;
    }
    bool QueueManagerRedis::DequeuePlayer(const dm::Uuid& uuid) {
        long long res;
        res = redis_->lrem(redis_data_name_, 0, uuid);
        return res > 0;
    }
    std::optional<std::string> QueueManagerRedis::PopPlayer() {
        sw::redis::OptionalString os;
        os = redis_->rpop(redis_data_name_);
        if (os)
            return *os;
        return std::nullopt;
    }
    long long QueueManagerRedis::GetLength()  {
        return redis_->llen(redis_data_name_);
    }
    std::vector<dm::Uuid> QueueManagerRedis::GetQueue(int start, int end) {
        std::vector<std::string> strs;
        redis_->lrange(redis_data_name_, start, end, std::back_inserter(strs));
        return strs;
    }
}