#pragma once
#include <map>
#include <memory>
#include <mutex>
#include "user_data.hpp"
#include "http_types.hpp"

namespace notification_system{

    class NetworkNotifier{
    public:
        enum StatusCode{
            Ok, // send as usual when success
            PollClosed, // poll is closed due server error or if 
            //user change poll session; then additional_data is an error description 
        };
        // std string is an additional data. can be enterprited as desired
        using AdditionalData = std::string;
        struct PollData{
            AdditionalData additional_data;
            bool unsubscribe = true;
            StatusCode code = StatusCode::Ok;
        };
        using LongPollResponser = std::function<void(StatusCode, AdditionalData)>;
        using Ptr = std::shared_ptr<NetworkNotifier>;


        NetworkNotifier(NetworkNotifier &other) = delete;
        void operator=(const NetworkNotifier &) = delete;

        static NetworkNotifier::Ptr GetInstance();

        bool Subscribe(const dm::Uuid& uuid, LongPollResponser&& responser);
        bool Unsubscribe(const dm::Uuid& uuid, const std::string& reason = "");

        bool Notify(const dm::Uuid& uuid, const PollData& poll_data);
    private:
        static NetworkNotifier::Ptr pinstance_;
        static std::mutex mutex_;
        NetworkNotifier() ;

        std::map<dm::Uuid, LongPollResponser> requests_;
        std::map<dm::Uuid, PollData> wait_for_poll_;
    };
}
namespace notif = notification_system;