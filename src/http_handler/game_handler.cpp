#include "game_handler.hpp"
#include "api_function_director.hpp"
#include "queue_notifier.hpp"
#include "session_state_notifier.hpp"
#include "send_manager.hpp"
#include "send_manager_gm.hpp"
#include "send_manager_sm.hpp"
#include "serializer_game.hpp"
#include "spdlog/spdlog.h"

#define BIND_API(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2) 
#define BIND_GAME(func) (ApiGameFunc)std::bind( func, gm_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) 

namespace http_handler{
    GameHandler::GameHandler(HandlerParameters handler_parameters)
    :ApiHandler(),
     iqm_(handler_parameters.queue_manager),
     gm_(handler_parameters.game_manager),
     tm_(handler_parameters.token_manager),
     sm_(handler_parameters.session_manager),
     um_(handler_parameters.user_manager)
      {}

    void GameHandler::Init() {
        ApiFunctionsParse();
    }
    void GameHandler::ApiFunctionsParse() {
        ApiFunctionDirector afd(tm_);
        request_to_executor_ = {
            {"/api/game/enqueue", afd.GetEnqueue(BIND_API(&GameHandler::ApiEnqueue))},
            {"/api/game/wait_for_opponent", afd.GetWaitForOpponent(BIND_API(&GameHandler::ApiWaitForOpponent))},
            {"/api/game/session_state", afd.GetSessionState(BIND_API(&GameHandler::ApiSessionState))},
            {"/api/game/session_state_change", afd.GetSessionStateChange(BIND_API(&GameHandler::ApiSessionStateChange))},
            {"/api/game/move", afd.GetMove(BIND_API(&GameHandler::ApiMove))},
            {"/api/game/game_consts", afd.GetGameConsts(BIND_API(&GameHandler::ApiGameConsts))},
        };
    }
    void GameHandler::ApiEnqueue(SessionData&& rns, const RequestData& rd){
        um::Uuid uuid = rd.uuid;

        auto user = um_->GetByUuid(uuid);
        if(!user)
            return SendInvalidToken(rns);

        if(auto sid = gm_->HasPlayer(user->login))
            return SendInTheMatch(rns, *sid);
            
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
        auto url = ParseUrlParameters(rns.request);
        if(url.size() != 2 || !url.contains("from_move") || !url.contains("sessionId"))
            return SendWrongUrlParameters(rns);
        gm::SessionId sid = url.at("sessionId");
        int from_move;
        try{
            from_move = std::stoi(url.at("from_move"));
        }
        catch(std::exception& ex){
            return SendWrongUrlParameters(rns);
        }

        using Notif = notif::SessionStateNotifier;
        bool res = Notif::GetInstance()->ChangePoll(uuid, sid, 
        [rns,sid,sm = sm_, from_move](Notif::PollStatus status, gm::EventListWrapper::OptCPtr events){
            if(status == Notif::PollStatus::Ok && !events)
                status = Notif::PollStatus::NotRelevant; 
            switch(status){
            case Notif::PollStatus::Ok:
                    SendEvents(rns, **events, from_move);    
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
        if(!res)
            return DefineSessionState(rns, sid);
    }

    void GameHandler::ApiMove(SessionData&& rns, const RequestData& rd) {
        const um::Uuid& uuid = rd.uuid;
        auto sidopt = ParseUrlSessionId(rns.request);
        if(!sidopt)
            return SendWrongUrlParameters(rns);
        auto sid = *sidopt;
        auto user_data = um_->GetByUuid(uuid);
        if(!user_data)
            return SendAccessDenied(rns);
        auto res = gm_->HasPlayer(user_data->login, sid);
        if (!res)
            return DefineSessionState(rns, sid);
        if (!res.value())
            return SendAccessDenied(rns);
        
        using Status = gm::GameApiStatus;
        std::optional<Status> status;

        std::optional<gm::MoveData> md = serializer::DeserializeMoveData(rns.request.body());
        if(!md)
            return SendWrongBodyData(rns);

        status = gm_->ApiMove(user_data->login, sid, *md);
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

    void GameHandler::ApiGameConsts(SessionData&& rns, const RequestData& rd) {
        std::string res = serializer::SerializeGameConst();
        return Send(rns, http::status::ok, std::move(res));
    }
}
