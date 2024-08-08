#include "game_handler.hpp"
#include "api_function_director.hpp"
#include "queue_notifier.hpp"
#include "session_state_notifier.hpp"
#include "send_manager.hpp"
#include "send_manager_gm.hpp"
#include "send_manager_sm.hpp"
#include "serializer_game.hpp"
#include "spdlog/spdlog.h"

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2) 

namespace http_handler{
    GameHandler::GameHandler(HandlerParameters handler_parameters)
    :ApiHandler(),
     iqm_(handler_parameters.queue_manager),
     gm_(handler_parameters.game_manager),
     tm_(handler_parameters.token_manager),
     sm_(handler_parameters.session_manager)
      {}

    void GameHandler::Init() {
        ApiFunctionsParse();
    }
    void GameHandler::ApiFunctionsParse() {
        ApiFunctionDirector afd(tm_);
        request_to_executor_ = {
            {"/api/game/enqueue", afd.GetEnqueue(BIND(&GameHandler::ApiEnqueue))},
            {"/api/game/wait_for_opponent", afd.GetWaitForOpponent(BIND(&GameHandler::ApiWaitForOpponent))},
            {"/api/game/session_state", afd.GetSessionState(BIND(&GameHandler::ApiSessionState))},
            {"/api/game/session_state_change", afd.GetSessionStateChange(BIND(&GameHandler::ApiSessionStateChange))},
            {"/api/game/move", afd.GetMove(BIND(&GameHandler::ApiMove))},
            {"/api/game/resign", afd.GetResign(BIND(&GameHandler::ApiResign))},
        };
    }
    void GameHandler::ApiEnqueue(SessionData&& rns, const RequestData& rd){
        um::Uuid uuid = rd.uuid;

        if(gm_->HasPlayer(uuid))
            return SendInTheMatch(rns);
            
        bool res = iqm_->EnqueuePlayer(uuid);
        if (!res)
            return SendCantEnqueue(rns);
        return SendSuccess(rns);
    }
    void GameHandler::ApiWaitForOpponent(SessionData&& rns, const RequestData& rd){
        um::Uuid uuid = rd.uuid;
        using Notif = notification_system::QueueNotifier;
        Notif::GetInstance()->Subscribe(uuid, 
            [rns = std::move(rns)](Notif::PollStatus code, const std::string& add_data){
                switch(code){
                    case Notif::PollStatus::Ok:
                        SendSessionId(rns, add_data); // add_data is sessionID
                    break;
                    case Notif::PollStatus::PollClosed:
                        SendPollClosed(rns, add_data); // add_data is error description
                    break;
                }
            });
    }
    void GameHandler::ApiSessionState(SessionData&& rns, const RequestData& rd){
        um::Uuid uuid = rd.uuid;
        
        auto sidopt = ParseUrlSessionId(rns.request);
        if(!sidopt)
            return SendWrongUrlParameters(rns);
        auto sid = *sidopt;
        gm::State::OptCPtr state = gm_->GetState(sid);
        if(!state){
            if(std::optional<session_manager::PublicSessionData> sd = sm_->GetPublicLine(sid))
                return SendFinishedState(rns, *sd);
            return SendWrongSessionId(rns);
        }
        return SendGameState(rns, **state);
    }

    void GameHandler::ApiSessionStateChange(SessionData&& rns, const RequestData& rd){
        um::Uuid uuid = rd.uuid;

        auto sidopt = ParseUrlSessionId(rns.request);
        if(!sidopt)
            return SendWrongUrlParameters(rns);
        auto sid = *sidopt;
        if(!gm_->HasSession(sid)) 
            return DefineSessionState(rns, sid);

        using Notif = notif::SessionStateNotifier;
        Notif::GetInstance()->ChangePoll(uuid, sid, 
        [rns = std::move(rns),sid,sm = sm_](Notif::PollStatus status, gm::State::OptCPtr state){
            switch(status){
            case Notif::PollStatus::Ok:
                    SendGameState(rns, **state);    
                break;
            case Notif::PollStatus::NotRelevant:
                if(std::optional<session_manager::PublicSessionData> sd = sm->GetPublicLine(sid))
                    SendFinishedState(rns, *sd);
                else
                    SendWrongSessionId(rns);
                break;

            case Notif::PollStatus::PollClosed:
                SendPollClosed(rns, "SessionStateNotifier poll replaced by other");    
                break;
            }
        });
    }

    void GameHandler::ApiMove(SessionData&& rns, const RequestData& rd) {
        um::Uuid uuid = rd.uuid;
        auto sidopt = ParseUrlSessionId(rns.request);
        if(!sidopt)
            return SendWrongUrlParameters(rns);
        auto sid = *sidopt;
        auto res = gm_->HasPlayer(uuid, sid);
        if (!res)
            return DefineSessionState(rns, sid);
        if (!res.value())
            return SendAccessDenied(rns);
        
        using Status = gm::Session::GameApiStatus;
        using Type = gm::Session::MoveType;
        std::optional<Type> pmt = serializer::DefinePlayerMove(rns.request.body());
        if(!pmt.has_value())
            return SendWrongBodyData(rns);
        std::optional<Status> status;
        switch (*pmt){
        case Type::Walk:
            std::optional<gm::Session::WalkData> data 
                = serializer::DeserializePlayerWalk(rns.request.body());
            if(!data.has_value())
                return SendWrongBodyData(rns);
            status = gm_->ApiWalk(uuid, sid, *data);
            break;
        }
        if(!status)
            return DefineSessionState(rns, sid);
        switch(*status){
        case Status::Ok:
            return SendSuccess(rns);
        case Status::NotYourMove:
            return SendNotYourMove(rns);
        case Status::WrongMove:
            return SendWrongMove(rns);
        }
    }

    void GameHandler::ApiResign(SessionData&& rns, const RequestData& rd) {
        um::Uuid uuid = rd.uuid;
        auto sidopt = ParseUrlSessionId(rns.request);
        if(!sidopt) 
            return SendWrongUrlParameters(rns);
        auto sid = *sidopt;

        auto res = gm_->ApiResign(uuid, sid);
        if(!res.has_value())
            return DefineSessionState(rns, sid);
        if(!res.value())
            return SendAccessDenied(rns);

        return SendSuccess(rns);
        
    }

    std::optional<gm::SessionId> GameHandler::ParseUrlSessionId(const HttpRequest& request) {
        auto map = ParseUrlParameters(request);
        if(map.size() != 1 || !map.contains("sessionId"))
            return std::nullopt;
        return map.at("sessionId");
    }
    void GameHandler::DefineSessionState(SessionData rns, const gm::SessionId& sid){
        if(std::optional<session_manager::PublicSessionData> sd = sm_->GetPublicLine(sid))
            return SendSessionFinished(rns);
        return SendWrongSessionId(rns);
    }
}
