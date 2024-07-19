#include "game_handler.hpp"
#include "api_function_director.hpp"
#include "network_notifier.hpp"
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
        };
    }
    void GameHandler::ApiEnqueue(SessionData rns){
        auto token = SenderAuthentication(rns.request);
        auto uuid = tm_->GetUuidByToken(token);
        if (!uuid)
            return responser_.SendCantEnqueue(rns);
        bool res = iqm_->EnqueuePlayer(*uuid);
        if (!res)
            return responser_.SendCantEnqueue(rns);
        return responser_.SendSuccess(rns);
    }
    void GameHandler::ApiWaitForOpponent(SessionData&& rns){
        auto token = SenderAuthentication(rns.request);
        auto uuid = tm_->GetUuidByToken(token);
        if (!uuid)
            return responser_.SendNoSuchUser(rns);
        notification_system::NetworkNotifier::GetInstance()->Subscribe(*uuid, 
            [resp = responser_, rns = std::move(rns)](const std::string& session_id){
                resp.SendSessionId(rns, session_id);
            });
    }
}