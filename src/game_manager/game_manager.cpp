#include "game_manager.hpp"
#include "session_id_generator.hpp"
#include "network_notifier.hpp"
#include "response_builder.hpp"

namespace game_manager{
    GameManager::GameManager(dm::IUserDataManager::Ptr udm)
        :udm_(udm){}

    bool GameManager::CreateSession(dm::Uuid&& player1, dm::Uuid&& player2){
        SessionId si = GenerateSessionId();
        sessions_.emplace(si, Session{std::move(player1), std::move(player2)});
        http_handler::ResponseBuilder<http::string_body> rb;
        notif::NetworkNotifier::GetInstance()->Notify(player1, {.additional_data=si });
        notif::NetworkNotifier::GetInstance()->Notify(player2, {.additional_data=si });
        return sessions_.contains(si);
    }

    bool GameManager::HasPlayer(const dm::Uuid& uuid) {
        for(std::pair<SessionId, Session> pair : sessions_){
            if (pair.second.HasPlayer(uuid))
                return true;
        }
        return false;
    }

    //ingame api
    bool GameManager::Ping(const dm::Uuid& player_id, const SessionId& session_id){
        if (sessions_.contains(session_id)){
            return sessions_.at(session_id).Ping(player_id);
        }
        return false;
    }

    SessionId GameManager::GenerateSessionId(){
        SessionIdGenerator sig;
        return sig.Generate();
    }
}

