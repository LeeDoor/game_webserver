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

    bool NetworkNotifier::Subscribe(const dm::Uuid& uuid, std::shared_ptr<beast::tcp_stream> session) {
        sessions_.emplace(uuid, session);
        return sessions_.contains(uuid);
    }
    bool NetworkNotifier::Unsubscribe(const dm::Uuid& uuid) {
        sessions_.erase(uuid);
        return !sessions_.contains(uuid);
    }

    bool NetworkNotifier::Send(const dm::Uuid& uuid, StringResponse&& response) {
        if(sessions_.contains(uuid)){
            http::async_write(*sessions_[uuid], std::move(response),
                [](beast::error_code ec, std::size_t bytes_written) {
                    if(ec) {
                        spdlog::error("NetworkNotifier error sending");
                    }
                });
            return true;
        }
        return false;
    }
}