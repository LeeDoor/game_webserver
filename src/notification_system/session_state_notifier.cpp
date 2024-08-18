#include "session_state_notifier.hpp"
#include "spdlog/spdlog.h"
namespace notification_system{
    SessionStateNotifier::SessionStateNotifier(gm::GameManager::Ptr gm)
        : gm_(gm){}

    SessionStateNotifier::Ptr SessionStateNotifier::pinstance_{nullptr};
    std::mutex SessionStateNotifier::mutex_;


    Lock<SessionStateNotifier::Ptr> SessionStateNotifier::GetInstance() {
        mutex_.lock();
        if (pinstance_ == nullptr)
            throw std::runtime_error("SessionStateNotifier not initialized. call SessionStateNotifier::Init");
        mutex_.unlock();
        return {mutex_, pinstance_};
    }
    void SessionStateNotifier::Init(gm::GameManager::Ptr gm) {
        std::lock_guard<std::mutex> lock(mutex_);
        //private constructor, need to create manually
        pinstance_ = std::shared_ptr<SessionStateNotifier>(new SessionStateNotifier(gm));
    }

    bool SessionStateNotifier::Subscribe(const um::Uuid& uuid, const gm::SessionId& sid) {
        sessions_[sid]; // creating session with sid
        if (sessions_.at(sid).users_responser.contains(uuid)){
            return false;
        }
        sessions_.at(sid).users_responser[uuid] = std::nullopt;
        spdlog::info("{} subscribed to {}", uuid, sid);
        return true;
    }
    bool SessionStateNotifier::Unsubscribe(const gm::SessionId& sid) {
        if(!sessions_.contains(sid))
            return false;
        // everyone who we are waiting for, are moved to send_and_close_
        auto pw = sessions_.at(sid).poll_waiting;
        for(auto uuid : pw){
            send_and_close_[{sid, uuid}] = GetEvents(sid);
        }

        // everyone who is waiting for response, gets NOT_RELEVANT
        auto ur = sessions_.at(sid).users_responser;
        for(auto pair : ur){
            if(pair.second.has_value()) {
                (*pair.second)(PollStatus::NotRelevant, std::nullopt);
            }
        }
        sessions_.erase(sid);
        return true;
    }
    bool SessionStateNotifier::ChangePoll(const um::Uuid& uuid, const gm::SessionId& sid, Responser&& responser) {
        if(send_and_close_.contains({sid, uuid})){
            auto events = send_and_close_.at({sid, uuid});
            responser(PollStatus::Ok, events);
            send_and_close_.erase({sid, uuid});
            return true;
        }

        if(!sessions_.contains(sid))
            return false;

        if(!sessions_.at(sid).users_responser.contains(uuid))
            Subscribe(uuid, sid);

        std::vector<um::Uuid>& pw = sessions_.at(sid).poll_waiting;
        const std::vector<um::Uuid>::iterator& it = std::find(pw.begin(), pw.end(), uuid);
        if (it != pw.end()){ // found in poll_waiting
            auto events = GetEvents(sid);
            responser(PollStatus::Ok, events);
            spdlog::info("{} player got call about {} using poll waiting", uuid, sid);
            pw.erase(it);
            return true;
        }

        if (sessions_.at(sid).users_responser.at(uuid).has_value()){
            //breaking previous poll if it is
            (*sessions_.at(sid).users_responser.at(uuid))(PollStatus::PollClosed, std::nullopt);
            spdlog::info("{} player got call about CLOSING {}", uuid, sid);
        }

        sessions_.at(sid).users_responser.at(uuid) = std::move(responser);
        return true;
    }
    bool SessionStateNotifier::Notify(const gm::SessionId& sid) {
        if(!sessions_.contains(sid))
            return false;
        gm::EventListWrapper::OptCPtr events = GetEvents(sid);
        for(const std::pair<um::Uuid, ResponserOpt>& pair : sessions_.at(sid).users_responser){
            if(!pair.second.has_value()){
                auto& pw = sessions_.at(sid).poll_waiting;
                auto it = std::find(pw.begin(), pw.end(), pair.first);
                if(it != pw.end())
                    continue;

                sessions_.at(sid).poll_waiting.push_back(pair.first);
                spdlog::info("{} player got added to poll waiting of {}", pair.first, sid);
                continue;
            }
            (*pair.second)(PollStatus::Ok, events);
            spdlog::info("{} player got call about {} using standard Notify()", pair.first, sid);
            sessions_.at(sid).users_responser[pair.first] = std::nullopt;
        }
        return true;
    }
    bool SessionStateNotifier::SessionCreated(const um::Uuid& player1, const um::Uuid& player2, const gm::SessionId& sid){
        Subscribe(player1, sid);
        Subscribe(player2, sid);
        return true;
    }

    gm::EventListWrapper::OptCPtr SessionStateNotifier::GetEvents(const gm::SessionId& sid){
        return gm_->GetEvents(sid);
    }
}