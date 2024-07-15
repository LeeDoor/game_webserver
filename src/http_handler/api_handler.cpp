#include "api_handler.hpp"
#include "get_token_from_header.hpp"
#include "api_function_director.hpp"
#include "spdlog/spdlog.h"
#include <fstream>

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1) 

#define STREAM_POS(stream_name) std::to_string(static_cast<long long>(stream_name.tellg()))
namespace http_handler {
    ApiHandler::ApiHandler(HandlerParameters handler_parameters) 
        :   serializer_(handler_parameters.serializer), 
            responser_(handler_parameters.serializer), 
            udm_(handler_parameters.user_data_manager),
            tm_(handler_parameters.token_manager){}

    void ApiHandler::Init(){
        ApiFunctionsParse();
    }

    void ApiHandler::Handle(HttpRequest&& request, SessionFunctions&& session_functions){
        std::string function = static_cast<std::string>(request.target());
        // removing url parameters
        std::string::size_type pos = function.find('?');
        function = function.substr(0, pos);

        SessionData rns {request, session_functions};
        if(request.method() == http::verb::options){
            ResponseBuilder<http::string_body> builder;
            http::response<http::string_body> product = 
                builder.Allow({http::verb::get, http::verb::head, http::verb::post})
                .Status(status::ok)
                .GetProduct();
            product.set(http::field::access_control_allow_origin, "*");
            product.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
            rns.session_functions.send_string(std::move(product));
            return;
        }
        if(request_to_executor_.contains(function)) {
            ApiFunctionExecutor& executor = request_to_executor_.at(function);
            ApiStatus result = executor.Execute(rns);
            if(result != ApiStatus::Ok)
                return responser_.HandleApiError(result, executor, rns);
        }
        else{
            responser_.SendWrongApiFunction(rns);
        }
    }

    void ApiHandler::ApiFunctionsParse () {
        ApiFunctionDirector afd(serializer_, tm_);
        request_to_executor_ = {
            {"/api/register", afd.GetRegister(BIND(&ApiHandler::ApiRegister))},
            {"/api/login", afd.GetLogin(BIND(&ApiHandler::ApiLogin))},
            {"/api/profile", afd.GetProfile(BIND(&ApiHandler::ApiGetProfileData))},
            {"/api/debug/player_tokens", afd.GetPlayerTokens(BIND(&ApiHandler::ApiGetPlayerTokens))},
            {"/api/debug/user_data", afd.GetUserData(BIND(&ApiHandler::ApiGetUserData))},
            {"/api/debug/matchmaking_queue", afd.GetMatchmakingQueue(BIND(&ApiHandler::ApiGetMMQueue))},
        };
    }

    void ApiHandler::ApiRegister(SessionData rns) {
        std::optional<RegistrationData> rd;
        rd = serializer_->DeserializeRegData(rns.request.body());
        if(!rd.has_value()) {
            return responser_.SendWrongBodyData(rns);
        }
        if(!ValidateRegData(*rd)){
            return responser_.SendWrongLoginOrPassword(rns);
        }
        bool add_line_res = udm_->AddLine(*rd);
        if(!add_line_res){
            return responser_.SendLoginTaken(rns);
        }
        return responser_.SendSuccess(rns);
    }
    void ApiHandler::ApiLogin(SessionData rns) {
        std::optional<RegistrationData> rd;
        rd = serializer_->DeserializeRegData(rns.request.body());
        if(!rd.has_value()) {
            return responser_.SendWrongBodyData(rns);
        }
        if(!ValidateRegData(*rd)){
            return responser_.SendWrongLoginOrPassword(rns);
        }
        std::optional<dm::UserData> ud;
        ud = udm_->GetByLoginPassword(std::move(rd->login), std::move(rd->password));
        if(!ud){
            return responser_.SendNoSuchUser(rns);
        }
        tokenm::Token token = tm_->GenerateToken();
        bool res = tm_->AddTokenToUuid(token, ud->uuid);
        if (!res)
            return responser_.SendCantLogin(rns);
        return responser_.SendToken(rns, token);
    }
    void ApiHandler::ApiGetProfileData(SessionData rns){
        auto token = SenderAuthentication(rns.request);
        auto uuid = tm_->GetUuidByToken(token);
        auto user_data = udm_->GetByUuid(*uuid);
        if(!user_data)
            return responser_.SendTokenToRemovedPerson(rns);
            
        return responser_.SendUserData(rns, *user_data);
    }

    void ApiHandler::ApiGetPlayerTokens(SessionData rns){
        std::map<token_manager::Token, dm::Uuid> map = tm_->GetValue();
        std::string tm_string = serializer_->SerializeTokenToUuid(map);
        return responser_.Send(rns, http::status::ok, tm_string);
    }

    void ApiHandler::ApiGetUserData(SessionData rns) {
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

    void ApiHandler::ApiGetMMQueue(SessionData rns) {
        /*const std::vector<dm::Uuid>& queue = iqm_->GetQueue();
        std::string queue_string = serializer_->SerializeUuids(queue);
        return responser_.Send(rns, status::ok, queue_string);*/
    }

    tokenm::Token ApiHandler::SenderAuthentication(const HttpRequest& request) {
        auto auth_iter = request.find(http::field::authorization);
        std::optional<tokenm::Token> token = GetTokenFromHeader(auth_iter->value().to_string());
        return *token;
    }

    std::map<std::string, std::string> ApiHandler::ParseUrlParameters(const HttpRequest& request){
        std::map<std::string, std::string> res;
        std::string function = request.target().to_string();
        std::string::size_type pos = function.find('?');
        if (pos == std::string::npos)
            return res;
        std::string params = function.substr(pos + 1);
        pos = params.find('&');
        std::string pair_s;
        do {
            pair_s = params.substr(0, pos);
            params.erase(0, pos + 1);
            res.insert(ParseUrlPair(std::move(pair_s)));
        } while ((pos = params.find('&')) != std::string::npos);
        res.insert(ParseUrlPair(std::move(params)));
        return res;
    }
    std::pair<std::string, std::string> ApiHandler::ParseUrlPair(std::string&& pair) {
        // if line does not have equality sign (e.g. "/api/function?parameter1&parameter2")
        // then these parameters are being parsed as 
        // {"parameter1":"parameter1", "parameter2":"parameter2"}
        return {pair.substr(0, pair.find('=')), pair.substr(pair.find('=') + 1)};
    }
    
} // http_handler