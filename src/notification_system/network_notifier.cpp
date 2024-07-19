#include "network_notifier.hpp"
#include "session.hpp"
#include <algorithm>
#include <boost/beast.hpp>
#include "spdlog/spdlog.h"
namespace notification_system{

    NetworkNotifier::Ptr NetworkNotifier::pinstance_{nullptr};
    std::mutex NetworkNotifier::mutex_;

    NetworkNotifier::NetworkNotifier(){}

    NetworkNotifier::Ptr NetworkNotifier::GetInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
        {
            pinstance_ = std::shared_ptr<NetworkNotifier>(new NetworkNotifier());
        }
        return pinstance_;
    }

    bool NetworkNotifier::Subscribe(const dm::Uuid& uuid, LongPollResponser&& notifier) {
        if (requests_.contains(uuid)) 
            Unsubscribe(uuid, "new poll connected");

        requests_.emplace(uuid, std::move(notifier));

        if(wait_for_poll_.contains(uuid)){
            Notify(uuid, wait_for_poll_[uuid]);
            wait_for_poll_.erase(uuid);
        }
        return requests_.contains(uuid);
    }
    bool NetworkNotifier::Unsubscribe(const dm::Uuid& uuid, const std::string& reason) {
        if(!reason.empty() && requests_.contains(uuid))
            Notify(uuid, {.additional_data=reason, .code=StatusCode::PollClosed});
        return requests_.erase(uuid);
    }

    bool NetworkNotifier::Notify(const dm::Uuid& uuid, const PollData& poll_data) {
        if(requests_.contains(uuid)){
            requests_[uuid](poll_data.code, poll_data.additional_data);

            if (poll_data.unsubscribe){
                return Unsubscribe(uuid);
            }
            return true;
        }
        wait_for_poll_.emplace(uuid, poll_data);
        return false;
    }
}