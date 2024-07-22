#include "queue_notifier.hpp"
#include <algorithm>
#include <boost/beast.hpp>
#include "spdlog/spdlog.h"
namespace notification_system{

    QueueNotifier::Ptr QueueNotifier::pinstance_{nullptr};
    std::mutex QueueNotifier::mutex_;

    QueueNotifier::QueueNotifier(){}

    QueueNotifier::Ptr QueueNotifier::GetInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
        {
            pinstance_ = std::shared_ptr<QueueNotifier>(new QueueNotifier());
        }
        return pinstance_;
    }

    bool QueueNotifier::Subscribe(const dm::Uuid& uuid, LongPollResponser&& notifier) {
        if (requests_.contains(uuid)) 
            Unsubscribe(uuid, "new poll connected");

        requests_.emplace(uuid, std::move(notifier));

        if(wait_for_poll_.contains(uuid)){
            Notify(uuid, wait_for_poll_[uuid]);
            wait_for_poll_.erase(uuid);
        }
        return requests_.contains(uuid);
    }
    bool QueueNotifier::Unsubscribe(const dm::Uuid& uuid, const std::string& reason) {
        if(!reason.empty() && requests_.contains(uuid))
            Notify(uuid, {.additional_data=reason, .code=StatusCode::PollClosed});
        return requests_.erase(uuid);
    }

    bool QueueNotifier::Notify(const dm::Uuid& uuid, const PollData& poll_data) {
        if(requests_.contains(uuid)){
            requests_[uuid](poll_data.code, poll_data.additional_data);

            return Unsubscribe(uuid);
        }
        wait_for_poll_.emplace(uuid, poll_data);
        return false;
    }
}