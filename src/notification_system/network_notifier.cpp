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
        requests_.emplace(uuid, std::move(notifier));
        return requests_.contains(uuid);
    }
    bool NetworkNotifier::Unsubscribe(const dm::Uuid& uuid) {
        requests_.erase(uuid);
        return !requests_.contains(uuid);
    }

    bool NetworkNotifier::Notify(const dm::Uuid& uuid, bool unsubscribe) {
        if(requests_.contains(uuid)){
            requests_[uuid]();

            if (unsubscribe){
                return Unsubscribe(uuid);
            }
            return true;
        }
        return false;
    }
}