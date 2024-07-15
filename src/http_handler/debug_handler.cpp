#include "debug_handler.hpp"
#include "api_function_director.hpp"

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1) 

namespace http_handler{

    DebugHandler::DebugHandler(HandlerParameters handler_parameters)
    :ApiHandler(handler_parameters),
    udm_(handler_parameters.user_data_manager),
    tm_(handler_parameters.token_manager),
    qm_(handler_parameters.queue_manager){}
    
    void DebugHandler::Init() {
        ApiFunctionsParse();
    }
    void DebugHandler::ApiFunctionsParse(){
        ApiFunctionDirector afd(serializer_, tm_);
        request_to_executor_ = {
            {"/api/debug/player_tokens", afd.GetPlayerTokens(BIND(&DebugHandler::ApiGetPlayerTokens))},
            {"/api/debug/user_data", afd.GetUserData(BIND(&DebugHandler::ApiGetUserData))},
            {"/api/debug/matchmaking_queue", afd.GetMatchmakingQueue(BIND(&DebugHandler::ApiGetMMQueue))},
        };
    }
    
    void DebugHandler::ApiGetPlayerTokens(SessionData rns){
        std::map<token_manager::Token, dm::Uuid> map = tm_->GetValue();
        std::string tm_string = serializer_->SerializeTokenToUuid(map);
        return responser_.Send(rns, http::status::ok, tm_string);
    }

    void DebugHandler::ApiGetUserData(SessionData rns) {
        std::map<std::string, std::string> map = ParseUrlParameters(rns.request);
        if (!(map.contains("login") && map.contains("password") && map.size() == 2 || map.contains("uuid") && map.size() == 1))
            return responser_.SendWrongUrlParameters(rns);
        if (map.contains("uuid")){
            std::optional<dm::UserData> ud = udm_->GetByUuid(map["uuid"]);
            if (!ud.has_value())
                return responser_.Send(rns, status::not_found, 
                    serializer_->SerializeError(
                        "user_not_found", "no user with provided uuid found"));
            return responser_.SendHiddenUserData(rns, *ud);
        }
        std::optional<dm::UserData> ud = udm_->GetByLoginPassword(map["login"], map["password"]);
        if (!ud.has_value())
            return responser_.Send(rns, status::not_found, 
                serializer_->SerializeError(
                    "user_not_found", "no user with provided login and password found"));
        return responser_.SendHiddenUserData(rns, *ud);
    }

    void DebugHandler::ApiGetMMQueue(SessionData rns) {
        const std::vector<dm::Uuid>& queue = qm_->GetQueue();
        std::string queue_string = serializer_->SerializeUuids(queue);
        return responser_.Send(rns, status::ok, queue_string);
    }
}