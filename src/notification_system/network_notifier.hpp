#pragma once
#include <map>
#include <memory>
#include <mutex>
#include "user_data.hpp"
#include "http_types.hpp"
#include <boost/asio/ip/tcp.hpp>

namespace notification_system{

    class NetworkNotifier{
    public:
        using Ptr = std::shared_ptr<NetworkNotifier>;
        NetworkNotifier(NetworkNotifier &other) = delete;
        void operator=(const NetworkNotifier &) = delete;

        static NetworkNotifier::Ptr GetInstance();

        bool Subscribe(const dm::Uuid& uuid, std::shared_ptr<beast::tcp_stream> session);
        bool Unsubscribe(const dm::Uuid& uuid);

        bool Send(const dm::Uuid& uuid, StringResponse&& response);
    private:
        static NetworkNotifier::Ptr pinstance_;
        static std::mutex mutex_;
        NetworkNotifier() ;

        std::map<dm::Uuid, std::shared_ptr<beast::tcp_stream>> sessions_;
    };
}
namespace notif = notification_system;