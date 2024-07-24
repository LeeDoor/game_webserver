#include "game_handler.hpp"
#include "api_function_director.hpp"
#include "queue_notifier.hpp"
#include "session_state_notifier.hpp"
#include "spdlog/spdlog.h"

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1) 

namespace http_handler{
    GameHandler::GameHandler(HandlerParameters handler_parameters)
    :ApiHandler(handler_parameters),
     iqm_(handler_parameters.queue_manager),
     gm_(handler_parameters.game_manager),
     tm_(handler_parameters.token_manager)
      {}

    void GameHandler::Init() {
        ApiFunctionsParse();
    }
    void GameHandler::ApiFunctionsParse() {
        ApiFunctionDirector afd(serializer_, tm_);
        request_to_executor_ = {
            {"/api/game/enqueue", afd.GetEnqueue(BIND(&GameHandler::ApiEnqueue))},
            {"/api/game/wait_for_opponent", afd.GetWaitForOpponent(BIND(&GameHandler::ApiWaitForOpponent))},
            {"/api/game/session_state", afd.GetSessionState(BIND(&GameHandler::ApiSessionState))},
            {"/api/game/session_state_change", afd.GetSessionStateChange(BIND(&GameHandler::ApiSessionStateChange))},
        };
    }
    void GameHandler::ApiEnqueue(SessionData rns){
        auto token = SenderAuthentication(rns.request);
        auto uuid = tm_->GetUuidByToken(token);

        if(gm_->HasPlayer(*uuid))
            return responser_.SendInTheMatch(rns);
            
        bool res = iqm_->EnqueuePlayer(*uuid);
        if (!res)
            return responser_.SendCantEnqueue(rns);
        return responser_.SendSuccess(rns);
    }
    void GameHandler::ApiWaitForOpponent(SessionData&& rns){
        auto token = SenderAuthentication(rns.request);
        auto uuid = tm_->GetUuidByToken(token);
        using Notif = notification_system::QueueNotifier;
        Notif::GetInstance()->Subscribe(*uuid, 
            [resp = responser_, rns = std::move(rns)](Notif::PollStatus code, const std::string& add_data){
                switch(code){
                    case Notif::PollStatus::Ok:
                        resp.SendSessionId(rns, add_data); // add_data is sessionID
                    break;
                    case Notif::PollStatus::PollClosed:
                        resp.SendPollClosed(rns, add_data); // add_data is error description
                    break;
                }
            });
    }
    void GameHandler::ApiSessionState(SessionData rns){
        auto token = SenderAuthentication(rns.request);
        auto uuid = tm_->GetUuidByToken(token);
        auto map = ParseUrlParameters(rns.request);
        if(map.size() != 1 || !map.contains("sessionId"))
            return responser_.SendWrongUrlParameters(rns);
        gm::SessionId sid = map.at("sessionId");
        gm::State::OptCPtr state = gm_->GetState(sid);
        if(!state.has_value())
            return responser_.SendWrongSessionId(rns);
        return responser_.SendGameState(rns, **state);
    }

    void GameHandler::ApiSessionStateChange(SessionData&& rns){
        auto token = SenderAuthentication(rns.request);
        auto uuid = tm_->GetUuidByToken(token);
        auto map = ParseUrlParameters(rns.request);
        if(map.size() != 1 || !map.contains("sessionId"))
            return responser_.SendWrongUrlParameters(rns);
        gm::SessionId sid = map.at("sessionId");
        using Notif = notif::SessionStateNotifier;
        Notif::GetInstance()->ChangePoll(*uuid, sid, 
        [rns = std::move(rns), resp = responser_](Notif::PollStatus status, gm::State::OptCPtr state){
            switch(status){
            case Notif::PollStatus::Ok:
                resp.SendGameState(rns, **state);    
                break;
            case Notif::PollStatus::PollClosed:
                resp.SendPollClosed(rns, "SessionStateNotifier poll replaced by other");    
                break;
            }
        });
    }
}