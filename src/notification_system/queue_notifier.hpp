#pragma once
#include <map>
#include <memory>
#include <mutex>
#include "user_data.hpp"
#include "http_types.hpp"

namespace notification_system{

    class QueueNotifier{
    public:
        using Ptr = std::shared_ptr<QueueNotifier>;
        enum PollStatus{
            Ok, // send as usual when success
            PollClosed, // poll is closed due server error or if 
            //user change poll session; then additional_data is an error description 
        };
        // std string is an additional data. can be enterprited as desired
        using AdditionalData = std::string;
        struct PollData{
            AdditionalData additional_data;
            PollStatus code = PollStatus::Ok;
        };
        using Responser = std::function<void(PollStatus, AdditionalData)>;


        QueueNotifier(QueueNotifier &other) = delete;
        void operator=(const QueueNotifier &) = delete;

        static QueueNotifier::Ptr GetInstance();

        bool Subscribe(const dm::Uuid& uuid, Responser&& responser);
        bool Unsubscribe(const dm::Uuid& uuid, const std::string& reason = "");

        bool Notify(const dm::Uuid& uuid, const PollData& poll_data);
    private:
        static QueueNotifier::Ptr pinstance_;
        static std::mutex mutex_;
        QueueNotifier() ;

        std::map<dm::Uuid, Responser> users_responser_;
        std::map<dm::Uuid, PollData> poll_waiting_;
    };
}
namespace notif = notification_system;