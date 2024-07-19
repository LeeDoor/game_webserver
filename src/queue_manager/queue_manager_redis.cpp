#include "queue_manager_redis.hpp"

namespace game_manager{
    QueueManagerRedis::QueueManagerRedis(std::string&& redis_data_name, std::string&& redis_set_name, RedisPtr redis)
    :   redis_queue_name_(redis_data_name),
        redis_set_name_(redis_set_name),
        redis_(redis){}

    bool QueueManagerRedis::EnqueuePlayer(const dm::Uuid& uuid) {
        bool contains = redis_->sismember(redis_set_name_, uuid);
        if (contains)
            return false;
        redis_->lpush(redis_queue_name_, uuid);
        redis_->sadd(redis_set_name_, uuid);
        Notify(EventType::OnEnqueue);
        return true;
    }
    bool QueueManagerRedis::DequeuePlayer(const dm::Uuid& uuid) {
        redis_->srem(redis_set_name_, uuid);
        long long res = redis_->lrem(redis_queue_name_, 0, uuid);
        return res > 0;
    }
    std::optional<std::string> QueueManagerRedis::PopPlayer() {
        sw::redis::OptionalString os;
        os = redis_->rpop(redis_queue_name_);
        if (!os)
            return std::nullopt;
        redis_->srem(redis_set_name_, *os);
        return *os;
    }
    long long QueueManagerRedis::GetLength()  {
        return redis_->llen(redis_queue_name_);
    }
    std::vector<dm::Uuid> QueueManagerRedis::GetQueue(int start, int end) {
        std::vector<std::string> strs;
        redis_->lrange(redis_queue_name_, start, end, std::back_inserter(strs));
        return strs;
    }
    void QueueManagerRedis::OnEnqueueSubscribe(IObserver::Ptr observer) {
        observers_.push_back(observer);
    }
    void QueueManagerRedis::Notify(EventType type) {
        for(auto it = observers_.begin(); it != observers_.end(); ++it){
            (*it)->Notify(type);
        }
    }
}