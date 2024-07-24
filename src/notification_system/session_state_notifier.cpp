#include "session_state_notifier.hpp"

namespace notification_system{
    SessionStateNotifier::SessionStateNotifier(gm::GameManager::Ptr gm)
        : gm_(gm){}

    SessionStateNotifier::Ptr SessionStateNotifier::pinstance_{nullptr};
    std::mutex SessionStateNotifier::mutex_;


    SessionStateNotifier::Ptr SessionStateNotifier::GetInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pinstance_ == nullptr)
            throw std::runtime_error("SessionStateNotifier not initialized. call SessionStateNotifier::Init");
        return pinstance_;
    }
    void SessionStateNotifier::Init(gm::GameManager::Ptr gm) {
        std::lock_guard<std::mutex> lock(mutex_);
        pinstance_ = std::shared_ptr<SessionStateNotifier>(new SessionStateNotifier(gm));
    }

    bool SessionStateNotifier::Subscribe(const dm::Uuid& uuid, const gm::SessionId& sid) {
        if (sessions_[sid].users_responser.contains(uuid)){
            return false;
        }
        sessions_[sid].users_responser[uuid] = std::nullopt;
        return true;
    }
    bool SessionStateNotifier::Unsubscribe(const dm::Uuid& uuid, const gm::SessionId& sid) {
        return sessions_[sid].users_responser.erase(uuid);
    }
    bool SessionStateNotifier::ChangePoll(const dm::Uuid& uuid, const gm::SessionId& sid, Responser&& responser) {
        if(!sessions_[sid].users_responser.contains(uuid))
            Subscribe(uuid, sid);

        std::vector<dm::Uuid>& pw = sessions_[sid].poll_waiting;
        const std::vector<dm::Uuid>::iterator& it = std::find(pw.begin(), pw.end(), uuid);
        if (it != pw.end()){ // found in poll_waiting
            responser(PollStatus::Ok, GetGameState(sid));
            pw.erase(it);
            return true;
        }

        if (sessions_[sid].users_responser[uuid].has_value()){
            //breaking previous poll if it is
            (*sessions_[sid].users_responser[uuid])(PollStatus::PollClosed, std::nullopt);
        }

        sessions_[sid].users_responser[uuid] = std::move(responser);
        return true;
    }
    bool SessionStateNotifier::Notify(const gm::SessionId& sid) {
        gm::State::OptCPtr state = GetGameState(sid);
        for(const std::pair<dm::Uuid, ResponserOpt>& pair : sessions_[sid].users_responser){
            if(!pair.second.has_value()){
                sessions_[sid].poll_waiting.push_back(pair.first);
                continue;
            }
            (*pair.second)(PollStatus::Ok, state);
        }
        return true;
    }

    gm::State::OptCPtr SessionStateNotifier::GetGameState(const gm::SessionId& sid){
        return gm_->GetState(sid);
    }
}