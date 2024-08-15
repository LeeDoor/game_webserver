#include "debug_handler.hpp"
#include "api_function_director.hpp"
#include "serializer_user.hpp"
#include "serializer_basic.hpp"
#include "serializer_game.hpp"
#include "send_manager.hpp"
#include "send_manager_um.hpp"

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2) 

namespace http_handler{

    DebugHandler::DebugHandler(HandlerParameters handler_parameters)
    :dm_(handler_parameters.user_manager),
    tm_(handler_parameters.token_manager),
    gm_(handler_parameters.game_manager),
    qm_(handler_parameters.queue_manager){}
    
    void DebugHandler::Init() {
        ApiFunctionsParse();
    }
    void DebugHandler::ApiFunctionsParse(){
        ApiFunctionDirector afd(tm_);
        request_to_executor_ = {
            {"/api/debug/player_tokens", afd.GetPlayerTokens(BIND(&DebugHandler::ApiGetPlayerTokens))},
            {"/api/debug/user", afd.GetUser(BIND(&DebugHandler::ApiGetUser))},
            {"/api/debug/matchmaking_queue", afd.GetMatchmakingQueue(BIND(&DebugHandler::ApiGetMMQueue))},
            {"/api/debug/set_state", afd.GetSetState(BIND(&DebugHandler::ApiSetState))},
        };
    }
    
    void DebugHandler::ApiGetPlayerTokens(SessionData&& rns, const RequestData& rd){
        std::map<token_manager::Token, um::Uuid> map = tm_->GetValue();
        std::string tm_string = serializer::Serialize(map);
        return Send(rns, http::status::ok, tm_string);
    }

    void DebugHandler::ApiGetUser(SessionData&& rns, const RequestData& rd) {
        std::map<std::string, std::string> map = ParseUrlParameters(rns.request);
        if (!(map.contains("login") && map.contains("password") && map.size() == 2 || map.contains("uuid") && map.size() == 1))
            return SendWrongUrlParameters(rns);
        if (map.contains("uuid")){
            std::optional<um::User> ud = dm_->GetByUuid(map["uuid"]);
            if (!ud.has_value())
                return Send(rns, status::not_found, 
                    serializer::SerializeError(
                        "user_not_found", "no user with provided uuid found"));
            return SendHiddenUser(rns, *ud);
        }
        std::optional<um::User> ud = dm_->GetByLoginPassword(map["login"], map["password"]);
        if (!ud.has_value())
            return Send(rns, status::not_found, 
                serializer::SerializeError(
                    "user_not_found", "no user with provided login and password found"));
        return SendHiddenUser(rns, *ud);
    }

    void DebugHandler::ApiGetMMQueue(SessionData&& rns, const RequestData& rd) {
        const std::vector<um::Uuid>& queue = qm_->GetQueue();
        std::string queue_string = serializer::Serialize(queue);
        return Send(rns, status::ok, queue_string);
    }

    void DebugHandler::ApiSetState(SessionData&& rns, const RequestData& rd) {
        std::map<std::string, std::string> map = ParseUrlParameters(rns.request);
        if(map.size() != 1 || !map.contains("sessionId"))
            return SendWrongUrlParameters(rns);
        auto state = serializer::DeserializeSessionState(rns.request.body());
        if(!state)
            return SendWrongBodyData(rns);
        if(!gm_->SetState(map.at("sessionId"), *state))
            return SendWrongSessionId(rns);
        return SendSuccess(rns);
    }
}