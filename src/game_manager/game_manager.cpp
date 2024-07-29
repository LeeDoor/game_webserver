#include "game_manager.hpp"
#include "session_id_generator.hpp"
#include "queue_notifier.hpp"
#include "session_state_notifier.hpp"
#include "response_builder.hpp"

namespace game_manager{
    GameManager::GameManager(dm::IUserManager::Ptr dm)
        :dm_(dm){}

    bool GameManager::CreateSession(dm::Uuid&& player1, dm::Uuid&& player2){
        SessionId si = GenerateSessionId();
        std::optional<dm::User> ud1 = dm_->GetByUuid(player1);
        std::optional<dm::User> ud2 = dm_->GetByUuid(player2);
        if(!ud1.has_value() || !ud2.has_value()) 
            return false;
        sessions_.emplace(si, Session{player1, ud1->login, player2, ud2->login}); 
        http_handler::ResponseBuilder<http::string_body> rb;
        notif::QueueNotifier::GetInstance()->Notify(player1, {.additional_data=si });
        notif::QueueNotifier::GetInstance()->Notify(player2, {.additional_data=si });

        notif::SessionStateNotifier::GetInstance()->SessionCreated(player1, player2, si);
        return sessions_.contains(si);
    }

    bool GameManager::HasSession(const SessionId& sid){
        return sessions_.contains(sid);
    }
    bool GameManager::HasPlayer(const dm::Uuid& uuid) {
        for(std::pair<SessionId, Session> pair : sessions_){
            if (pair.second.HasPlayer(uuid))
                return true;
        }
        return false;
    }
    bool GameManager::HasPlayer(const dm::Uuid& uuid, const SessionId& sessionId){
        if (sessions_.contains(sessionId))
            return sessions_.at(sessionId).HasPlayer(uuid);
        throw std::runtime_error("GameManager Should check if session exists");
    }
    State::OptCPtr GameManager::GetState(const SessionId& sessionId){
        if(sessions_.contains(sessionId))
            return sessions_.at(sessionId).GetState();
        return std::nullopt;
    }

    bool GameManager::ApiResign(const dm::Uuid& uuid, const gm::SessionId& sid) {
        if (!HasPlayer(uuid, sid))
            return false;
        sessions_.erase(sid);
        return true;
    }

    //ingame api
    Session::GameApiStatus GameManager::ApiWalk(const dm::Uuid& uuid, const SessionId& sid, const Session::WalkData& data){
        if (sessions_.contains(sid)){
            auto status = sessions_.at(sid).ApiWalk(uuid, data);
            if (status == Session::GameApiStatus::Ok)
                notif::SessionStateNotifier::GetInstance()->Notify(sid);
            return status;
        }
        throw std::runtime_error("GameManager Should check if session exists");
    }

    SessionId GameManager::GenerateSessionId(){
        SessionIdGenerator sig;
        return sig.Generate();
    }
}

