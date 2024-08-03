#include "debug_handler.hpp"
#include "api_function_director.hpp"
#include "serializer_user.hpp"
#include "serializer_basic.hpp"
#include "send_manager.hpp"
#include "send_manager_um.hpp"

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1) 

namespace http_handler{

    DebugHandler::DebugHandler(HandlerParameters handler_parameters)
    :dm_(handler_parameters.user_manager),
    tm_(handler_parameters.token_manager),
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
        };
    }
    
    void DebugHandler::ApiGetPlayerTokens(SessionData rns){
        std::map<token_manager::Token, um::Uuid> map = tm_->GetValue();
        std::string tm_string = serializer::Serialize(map);
        return Send(rns, http::status::ok, tm_string);
    }

    void DebugHandler::ApiGetUser(SessionData rns) {
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

    void DebugHandler::ApiGetMMQueue(SessionData rns) {
        const std::vector<um::Uuid>& queue = qm_->GetQueue();
        std::string queue_string = serializer::Serialize(queue);
        return Send(rns, status::ok, queue_string);
    }
}