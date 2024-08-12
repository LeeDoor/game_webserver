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

        sessions_.emplace(si, std::make_shared<Session>(player1, ud1->login, player2, ud2->login)); 
        notif::QueueNotifier::GetInstance()->Notify(player1, {.additional_data=si });
        notif::QueueNotifier::GetInstance()->Notify(player2, {.additional_data=si });

        notif::SessionStateNotifier::GetInstance()->SessionCreated(player1, player2, si);
        return sessions_.contains(si);
    }

    bool GameManager::HasSession(const SessionId& sid){
        return sessions_.contains(sid);
    }
    std::optional<SessionId> GameManager::HasPlayer(const um::Uuid& uuid) {
        for(std::pair<SessionId, Session::Ptr> pair : sessions_){
            if (pair.second->HasPlayer(uuid))
                return pair.first;
        }
        return std::nullopt;
    }
    std::optional<bool> GameManager::HasPlayer(const um::Uuid& uuid, const SessionId& sessionId){
        if (!sessions_.contains(sessionId))
            return std::nullopt;
        return sessions_.at(sessionId)->HasPlayer(uuid);
    }
    State::OptCPtr GameManager::GetState(const SessionId& sessionId){
        if(!sessions_.contains(sessionId))
            return std::nullopt;
        return sessions_.at(sessionId)->GetState();
    }

    std::optional<Session::GameApiStatus> GameManager::ApiMove(Session::MoveType mt, const um::Uuid& uuid, const gm::SessionId& sid, const Session::VariantData& data) {
        using MT = Session::MoveType;
        switch (mt){
        case MT::Resign: return ApiResign(uuid, sid);
        case MT::PlaceBomb: return ApiPlaceBomb(uuid, sid, std::get<Session::PlaceData>(data));
        case MT::Walk: return ApiWalk(uuid, sid, std::get<Session::PlaceData>(data));
        default: throw std::logic_error("MoveType not implemented in GameManager. see ApiMove");
        }
    }

    //ingame api

    std::optional<Session::GameApiStatus> GameManager::ApiResign(const um::Uuid& uuid, const gm::SessionId& sid) {
        if (!sessions_.contains(sid))
            return std::nullopt;
        auto status = sessions_.at(sid)->ApiResign(uuid);
        CheckStatus(sid, status);
        return status;
    }
    std::optional<Session::GameApiStatus> GameManager::ApiWalk(const um::Uuid& uuid, const SessionId& sid, const Session::PlaceData& data){
        if (!sessions_.contains(sid))
            return std::nullopt;
        auto status = sessions_.at(sid)->ApiWalk(uuid, data);
        CheckStatus(sid, status);
        return status;
    }
    std::optional<Session::GameApiStatus> GameManager::ApiPlaceBomb(const um::Uuid& uuid, const SessionId& sid, const Session::PlaceData& data) {
        if (!sessions_.contains(sid))
            return std::nullopt;
        auto status = sessions_.at(sid)->ApiPlaceBomb(uuid, data);
        CheckStatus(sid, status);
        return status;
    }

    void GameManager::CheckStatus(const SessionId& sid, Session::GameApiStatus status) {
        if (status == Session::GameApiStatus::Ok){
            notif::SessionStateNotifier::GetInstance()->Notify(sid);
            if (auto results = sessions_.at(sid)->GetResults()) {
                FinishSession(sid, *results);
            }
        }
    }
    bool GameManager::FinishSession(const SessionId& sid, const Session::Results& results) {
        sm_->AddLine({sid, results.winner, results.loser});

        notif::SessionStateNotifier::GetInstance()->Unsubscribe(results.winner, sid);
        notif::SessionStateNotifier::GetInstance()->Unsubscribe(results.loser, sid);

        sessions_.erase(sid);
        return true;
    }

    SessionId GameManager::GenerateSessionId(){
        SessionIdGenerator sig;
        return sig.Generate();
    }
}

 