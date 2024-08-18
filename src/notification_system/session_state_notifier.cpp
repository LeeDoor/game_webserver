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
        if (sessions_[sid].users_responser.contains(uuid)){
            return false;
        }
        sessions_[sid].users_responser[uuid] = std::nullopt;
        spdlog::info("{} subscribed to {}", uuid, sid);
        return true;
    }
    bool SessionStateNotifier::Unsubscribe(const um::Uuid& uuid, const gm::SessionId& sid) {
        if(!sessions_[sid].users_responser.contains(uuid))
            return false;
        if(!sessions_[sid].users_responser[uuid].has_value())
            return false;
        (*sessions_[sid].users_responser[uuid])(PollStatus::NotRelevant, std::nullopt);
        return true;
    }
    bool SessionStateNotifier::ChangePoll(const um::Uuid& uuid, const gm::SessionId& sid, Responser&& responser) {
        if(!sessions_[sid].users_responser.contains(uuid))
            Subscribe(uuid, sid);

        std::vector<um::Uuid>& pw = sessions_[sid].poll_waiting;
        const std::vector<um::Uuid>::iterator& it = std::find(pw.begin(), pw.end(), uuid);
        if (it != pw.end()){ // found in poll_waiting
            auto events = GetEvents(sid);
            responser(PollStatus::Ok, events);
            spdlog::info("{} player got call about {} using poll waiting", uuid, sid);
            pw.erase(it);
            return true;
        }

        if (sessions_[sid].users_responser[uuid].has_value()){
            //breaking previous poll if it is
            (*sessions_[sid].users_responser[uuid])(PollStatus::PollClosed, std::nullopt);
            spdlog::info("{} player got call about CLOSING {}", uuid, sid);
        }

        sessions_[sid].users_responser[uuid] = std::move(responser);
        return true;
    }
    bool SessionStateNotifier::Notify(const gm::SessionId& sid) {
        gm::EventListWrapper::OptCPtr events = GetEvents(sid);
        for(const std::pair<um::Uuid, ResponserOpt>& pair : sessions_[sid].users_responser){
            if(!pair.second.has_value()){
                sessions_[sid].poll_waiting.push_back(pair.first);
                spdlog::info("{} player got added to poll waiting of {}", pair.first, sid);
                continue;
            }
            (*pair.second)(PollStatus::Ok, events);
            spdlog::info("{} player got call about {} using standard Notify()", pair.first, sid);
            sessions_[sid].users_responser[pair.first] = std::nullopt;
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