#include "api_handler.hpp"
#include "get_token_from_header.hpp"
#include <iostream>
#include <fstream>

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1)    
#define STREAM_POS(stream_name) std::to_string(static_cast<long long>(stream_name.tellg()))
namespace http_handler {
    ApiHandler::ApiHandler(HandlerParameters handler_parameters) 
        :   serializer_(handler_parameters.serializer), 
            responser_(handler_parameters.serializer), 
            udm_(handler_parameters.user_data_manager),
            mm_queue_(handler_parameters.mm_queue),
            ttu_(handler_parameters.token_to_uuid),
            api_path_(handler_parameters.api_path){}

    void ApiHandler::Init(){
        ApiFunctionsParse();
    }

    void ApiHandler::HandleApiFunction(HttpRequest&& request, ResponseSender&& sender){
        std::string function = static_cast<std::string>(request.target());
        // removing url parameters
        std::string::size_type pos = function.find('?');
        function = function.substr(0, pos);

        RequestNSender rns {request, sender};
        if(request.method() == http::verb::options){
            ResponseBuilder<http::string_body> builder;
            http::response<http::string_body> product = 
                builder.Allow({http::verb::get, http::verb::head, http::verb::post})
                .Status(status::ok)
                .GetProduct();
            product.set(http::field::access_control_allow_origin, "*");
            product.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
            rns.sender.string(std::move(product));
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
        std::map<std::string, ExecutorFunction> executors{
            { "ApiGetProfileData",  BIND(&ApiHandler::ApiGetProfileData) },
            { "ApiRegister",        BIND(&ApiHandler::ApiRegister) },
            { "ApiLogin",           BIND(&ApiHandler::ApiLogin) },
            { "ApiEnqueue",         BIND(&ApiHandler::ApiEnqueue) },
            { "ApiGetPlayerTokens", BIND(&ApiHandler::ApiGetPlayerTokens) },
            { "ApiGetUserData",     BIND(&ApiHandler::ApiGetUserData) },
            { "ApiGetMMQueue",      BIND(&ApiHandler::ApiGetMMQueue) },
        };

        std::ifstream is;
        is.open(api_path_);
        ApiFunctionBuilder builder(serializer_);
        while (ApiFunctionParse(executors, is, builder));
        is.close();

    }
    bool ApiHandler::ApiFunctionParse(std::map<std::string, ExecutorFunction>& executors, std::ifstream& is, ApiFunctionBuilder& builder) {
        std::string target, function, param, cur_pos;
        std::vector<http::verb> verbs_vec;
        try{
            cur_pos = STREAM_POS(is); // method to track logic error if it is
            std::getline(is, target); // reading api target
            if (target.empty()) throw std::logic_error(cur_pos);

            cur_pos = STREAM_POS(is);
            std::getline(is, param); // reading methods
            std::stringstream verbs(param);
            if (verbs.eof()) throw std::logic_error(cur_pos);
            while (verbs >> param) // reading allowed (verbs)
                verbs_vec.push_back(http::string_to_verb(param));
            if (std::find(verbs_vec.begin(), verbs_vec.end(), http::verb::unknown) != verbs_vec.end()) 
                throw std::logic_error(cur_pos);
            
            builder.Methods(std::move(verbs_vec));

            cur_pos = STREAM_POS(is);
            std::getline(is, function);
            if (!executors.contains(function)) throw std::logic_error(cur_pos);
            // reading method name from file and mapping it with C++ function
            builder.ExecFunc(std::move(executors[function]));  

            cur_pos = STREAM_POS(is);
            std::getline(is, param); // reading custom parameters about function
            while (!param.empty()){
                if (param == "Authorization required")
                    builder.NeedAuthor(ttu_);
                else if (param == "Debug method")
                    builder.ForDebug();
                else
                    throw std::logic_error(cur_pos);
                if (is.eof()) {
                    request_to_executor_.emplace(target, builder.GetProduct());
                    return false;
                }
                std::getline(is, param).eof();
                cur_pos = STREAM_POS(is);
            }
            request_to_executor_.emplace(target, builder.GetProduct());
        }
        catch(std::exception& ex){
            int pos = std::atoi(ex.what());
            is.seekg(pos);
            std::getline(is, param);
            std::cout << "Unable to read API_functions.txt. defect line: " << param << std::endl;
            is.close();
            throw std::logic_error("Unable to read API_functions.txt");
        }
        return true;
    }

    void ApiHandler::ApiRegister(RequestNSender rns) {
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
    void ApiHandler::ApiLogin(RequestNSender rns) {
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
        tokenm::Token token = ttu_->GenerateToken();
        ttu_->AddTokenToUuid(token, ud->uuid);
        return responser_.SendToken(rns, token);
    }
    void ApiHandler::ApiGetProfileData(RequestNSender rns){
        auto token = SenderAuthentication(rns.request);
        auto uuid = ttu_->GetUuidByToken(token);
        auto user_data = udm_->GetByUuid(*uuid);
        if(!user_data)
            return responser_.SendTokenToRemovedPerson(rns);
            
        return responser_.SendUserData(rns, *user_data);
    }
    void ApiHandler::ApiEnqueue(RequestNSender rns){ 
        auto token = SenderAuthentication(rns.request);
        auto uuid = ttu_->GetUuidByToken(token);
        bool res = mm_queue_->EnqueuePlayer(*uuid);
        if (res){
            return responser_.SendSuccess(rns);
        }
        return responser_.SendCantEnqueue(rns);
    }

    void ApiHandler::ApiGetPlayerTokens(RequestNSender rns){
        const std::map<token_manager::Token, dm::Uuid>& map = ttu_->GetValue();
        std::string ttu_string = serializer_->SerializeTokenToUuid(map);
        return responser_.Send(rns, http::status::ok, ttu_string);
    }

    void ApiHandler::ApiGetUserData(RequestNSender rns) {
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

    void ApiHandler::ApiGetMMQueue(RequestNSender rns) {
        const std::vector<dm::Uuid>& queue = mm_queue_->GetQueue();
        std::string queue_string = serializer_->SerializeUuids(queue);
        return responser_.Send(rns, status::ok, queue_string);
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