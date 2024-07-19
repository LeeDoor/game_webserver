#include "game_manager.hpp"
#include "session_id_generator.hpp"
#include "network_notifier.hpp"
#include "response_builder.hpp"

namespace game_manager{

    bool GameManager::CreateSession(const dm::Uuid& player1, const dm::Uuid& player2){
        SessionId si = GenerateSessionId();
        sessions_[si]; // create new session
        http_handler::ResponseBuilder<http::string_body> rb;
        notif::NetworkNotifier::GetInstance()->Notify(player1, {.additional_data=si });
        notif::NetworkNotifier::GetInstance()->Notify(player2, {.additional_data=si });
        return sessions_.contains(si);
    }

    //ingame api
    bool GameManager::Ping(const dm::Uuid& player_id, const SessionId& session_id){
        if (sessions_.contains(session_id)){
            return sessions_[session_id].Ping(player_id);
        }
        return false;
    }

    GameManager::SessionId GameManager::GenerateSessionId(){
        SessionIdGenerator sig;
        return sig.Generate();
    }
}

