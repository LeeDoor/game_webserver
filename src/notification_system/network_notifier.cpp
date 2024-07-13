#include "network_notifier.hpp"
#include "session.hpp"
#include <algorithm>

namespace http_server{
    bool NetworkNotifier::Subscribe(const dm::Uuid& uuid, std::shared_ptr<Session> session) {
        sessions_.emplace(uuid, session);
        return sessions_.contains(uuid);
    }
    bool NetworkNotifier::Unsubscribe(const dm::Uuid& uuid) {
        sessions_.erase(uuid);
        return !sessions_.contains(uuid);
    }

    bool NetworkNotifier::Send(const dm::Uuid& uuid, StringResponse&& response) {
        std::shared_ptr<Session> session;
        if(sessions_.contains(uuid)){
            sessions_[uuid]->WriteOnce(std::move(response));
            return true;
        }
        return false;
    }
}