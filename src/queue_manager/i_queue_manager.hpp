#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "user.hpp"

namespace game_manager{
    class IQueueManager{
    public:
        enum EventType {
            OnEnqueue
        };
        class IObserver{
        public:
            using Ptr = std::shared_ptr<IObserver>;
            virtual void Notify(EventType type) = 0;
        };
        using Ptr = std::shared_ptr<IQueueManager>;
        
        //adds player to queue
        //returns: true - success; false - player already in queue
        virtual bool EnqueuePlayer(const dm::Uuid& uuid) = 0;
        //dequeues player from queue
        //returns: true - success; false - no player in queue
        virtual bool DequeuePlayer(const dm::Uuid& uuid) = 0;

        // pops player from queue and returns it.
        // std::nullopt if queue is empty
        virtual std::optional<dm::Uuid> PopPlayer() = 0;

        // returns size of queue
        virtual long long GetLength() = 0;

        // start and end are indices of first and last parsing objects
        // -1 is last, -2 is penultimate etc.
        virtual std::vector<dm::Uuid> GetQueue(int start = 0, int end = -1) = 0;

        virtual void OnEnqueueSubscribe(IObserver::Ptr observer) = 0;
    protected:
        virtual void Notify(EventType type) = 0;
    };
}