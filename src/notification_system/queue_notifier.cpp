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

    bool QueueNotifier::Subscribe(const um::Uuid& uuid, Responser&& notifier) {
        if (users_responser_.contains(uuid)) 
            Unsubscribe(uuid, "new poll connected");

        users_responser_.emplace(uuid, std::move(notifier));

        if(poll_waiting_.contains(uuid)){
            Notify(uuid, poll_waiting_[uuid]);
            poll_waiting_.erase(uuid);
        }
        return users_responser_.contains(uuid);
    }
    bool QueueNotifier::Unsubscribe(const um::Uuid& uuid, const std::string& reason) {
        if(!reason.empty() && users_responser_.contains(uuid))
            Notify(uuid, {.additional_data=reason, .code=PollStatus::PollClosed});
        return users_responser_.erase(uuid);
    }

    bool QueueNotifier::Notify(const um::Uuid& uuid, const PollData& poll_data) {
        if(users_responser_.contains(uuid)){
            users_responser_[uuid](poll_data.code, poll_data.additional_data);

            return Unsubscribe(uuid);
        }
        poll_waiting_.emplace(uuid, poll_data);
        return false;
    }
}