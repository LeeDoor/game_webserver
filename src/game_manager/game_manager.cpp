#include "game_manager.hpp"
#include "session_id_generator.hpp"
#include "queue_notifier.hpp"
#include "session_state_notifier.hpp"

namespace game_manager{
    GameManager::GameManager(um::IUserManager::Ptr um, sm::ISessionManager::Ptr sm)
        :dm_(um), sm_(sm){}

    bool GameManager::CreateSession(um::Uuid&& player1, um::Uuid&& player2){
        SessionId si = GenerateSessionId();
        std::optional<um::User> ud1 = dm_->GetByUuid(player1);
        std::optional<um::User> ud2 = dm_->GetByUuid(player2);
        if(!ud1.has_value() || !ud2.has_value()) 
            return false;

        sessions_.emplace(si, Session{player1, ud1->login, player2, ud2->login}); 
        notif::QueueNotifier::GetInstance()->Notify(player1, {.additional_data=si });
        notif::QueueNotifier::GetInstance()->Notify(player2, {.additional_data=si });

        notif::SessionStateNotifier::GetInstance()->SessionCreated(player1, player2, si);
        return sessions_.contains(si);
    }

    bool GameManager::HasSession(const SessionId& sid){
        return sessions_.contains(sid);
    }
    bool GameManager::HasPlayer(const um::Uuid& uuid) {
        for(std::pair<SessionId, Session> pair : sessions_){
            if (pair.second.HasPlayer(uuid))
                return true;
        }
        return false;
    }
    std::optional<bool> GameManager::HasPlayer(const um::Uuid& uuid, const SessionId& sessionId){
        if (!sessions_.contains(sessionId))
            return std::nullopt;
        return sessions_.at(sessionId).HasPlayer(uuid);
    }
    State::OptCPtr GameManager::GetState(const SessionId& sessionId){
        if(!sessions_.contains(sessionId))
            return std::nullopt;
        return sessions_.at(sessionId).GetState();
    }

    std::optional<bool> GameManager::ApiResign(const um::Uuid& uuid, const gm::SessionId& sid) {
        if (!sessions_.contains(sid))
            return std::nullopt;
        auto status = sessions_.at(sid).ApiResign(uuid);
        if(status != Session::GameApiStatus::Finish)
            return false;
        FinishSession(sid);
        return true;
    }

    //ingame api
    std::optional<Session::GameApiStatus> GameManager::ApiWalk(const um::Uuid& uuid, const SessionId& sid, const Session::WalkData& data){
        if (!sessions_.contains(sid))
            return std::nullopt;
        auto status = sessions_.at(sid).ApiWalk(uuid, data);
        if (status == Session::GameApiStatus::Ok)
            notif::SessionStateNotifier::GetInstance()->Notify(sid);
        return status;
    }

    bool GameManager::FinishSession(const SessionId& sid) {
        if (!sessions_.contains(sid))
            return false;
        const Session::Results& players = sessions_.at(sid).GetResults();
        sm_->AddLine({sid, players.winner, players.loser});

        notif::SessionStateNotifier::GetInstance()->Unsubscribe(players.winner, sid);
        notif::SessionStateNotifier::GetInstance()->Unsubscribe(players.loser, sid);

        sessions_.erase(sid);
        return true;
    }

    SessionId GameManager::GenerateSessionId(){
        SessionIdGenerator sig;
        return sig.Generate();
    }
}

