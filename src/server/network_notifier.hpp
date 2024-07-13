#pragma once
#include <map>
#include <memory>
#include "user_data.hpp"
#include "http_types.hpp"

namespace http_server{
    class Session;

    class NetworkNotifier{
    public:
        using Ptr = std::shared_ptr<NetworkNotifier>;

        bool Subscribe(const dm::Uuid& uuid, std::shared_ptr<Session> session);
        bool Unsubscribe(const dm::Uuid& uuid);

        void Send(const dm::Uuid& uuid, StringResponse&& response);
    private:
        std::map<dm::Uuid, std::shared_ptr<Session>> sessions_;
    };
}