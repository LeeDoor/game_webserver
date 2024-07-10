#include "queue_manager_redis.hpp"

namespace matchmaking_system{
    QueueManagerRedis::QueueManagerRedis(std::string&& redis_data_name, RedisPtr redis)
    :   redis_data_name_(redis_data_name),
        redis_(redis){}

    //adds player to queue
    //returns: true - success; false - player already in queue
    bool QueueManagerRedis::EnqueuePlayer(const dm::Uuid& uuid) {
        return false;
    }
    //dequeues player from queue
    //returns: true - success; false - no player in queue
    bool QueueManagerRedis::DequeuePlayer(const dm::Uuid& uuid) {
        return false;
    }

    // start and end are indices of first and last parsing objects
    // -1 is last, -2 is penultimate etc.
    std::vector<dm::Uuid> QueueManagerRedis::GetQueue(int start, int end) {
        return {};
    }
}