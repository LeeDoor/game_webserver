#pragma once
#include <map>
#include <memory>
#include <mutex>
#include "user_data.hpp"
#include "http_types.hpp"

namespace notification_system{

    class NetworkNotifier{
    public:
        using LongPollResponser = std::function<void()>;
        using Ptr = std::shared_ptr<NetworkNotifier>;

        NetworkNotifier(NetworkNotifier &other) = delete;
        void operator=(const NetworkNotifier &) = delete;

        static NetworkNotifier::Ptr GetInstance();

        bool Subscribe(const dm::Uuid& uuid, LongPollResponser&& responser);
        bool Unsubscribe(const dm::Uuid& uuid);

        bool Notify(const dm::Uuid& uuid, bool unsubscribe = true);
    private:
        static NetworkNotifier::Ptr pinstance_;
        static std::mutex mutex_;
        NetworkNotifier() ;

        std::map<dm::Uuid, LongPollResponser> requests_;
    };
}
namespace notif = notification_system;