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
        if (requests_.contains(uuid)) return false;

        requests_.emplace(uuid, std::move(notifier));

        if(wait_for_poll_.contains(uuid)){
            Notify(uuid, wait_for_poll_[uuid].additional_data, wait_for_poll_[uuid].unsubscribe);
        }
        return requests_.contains(uuid);
    }
    bool NetworkNotifier::Unsubscribe(const dm::Uuid& uuid) {
        return requests_.erase(uuid);
    }

    bool NetworkNotifier::Notify(const dm::Uuid& uuid, const AdditionalData& additional_data, bool unsubscribe) {
        if(requests_.contains(uuid)){
            requests_[uuid](additional_data);

            if (unsubscribe){
                return Unsubscribe(uuid);
            }
            return true;
        }
        wait_for_poll_.emplace(uuid, PollData{additional_data, unsubscribe});
        return false;
    }
}