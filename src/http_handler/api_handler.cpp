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
            uds_(handler_parameters.user_data_manager),
            mm_queue_(handler_parameters.mm_queue),
            ttu_(handler_parameters.token_to_uuid){}
    void ApiHandler::Init(){
        BuildTargetsMap();
    }

    void ApiHandler::HandleApiFunction(HttpRequest&& request, ResponseSender&& sender){
        std::string function = static_cast<std::string>(request.target());
        RequestNSender rns {request, sender};
        std::cout << function << std::endl;
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

    void ApiHandler::BuildTargetsMap() {
        std::map<std::string, ExecutorFunction> executors{
            { "ApiGetProfileData", BIND(&ApiHandler::ApiGetProfileData) },
        };

        std::ifstream is;
        is.open("API_functions.txt");
        std::string target, function, param;
        std::vector<http::verb> verbs_vec;
        ApiFunctionBuilder builder;
        
        try{
            std::getline(is, target); // reading api target
            if (target.empty()) throw std::logic_error(STREAM_POS(is));

            std::getline(is, param); // reading methods
            std::stringstream verbs(param);
            if (verbs.eof()) throw std::logic_error(STREAM_POS(is));
            while (verbs >> param)
                verbs_vec.push_back(http::string_to_verb(param));
            if (std::find(verbs_vec.begin(), verbs_vec.end(), http::verb::unknown) != verbs_vec.end()) 
                throw std::logic_error(STREAM_POS(is));
            builder.Methods(std::move(verbs_vec));

            std::getline(is, function);
            if (!executors.contains(function)) throw std::logic_error(STREAM_POS(is));
            builder.ExecFunc(std::move(executors[function]));  

            std::getline(is, param);
            while (!param.empty()){
                if (param == "Authorization required")
                    builder.NeedAuthor(ttu_);
                else if (param == "Debug method")
                    std::cout << "Debug method NOT IMPLEMENTED" << std::endl;
                else
                    throw std::logic_error(STREAM_POS(is));
                if(std::getline(is, param).eof()) break;
            }
            request_to_executor_.emplace(target, builder.GetProduct());
        }
        catch(std::exception ex){
            std::cout << "Unable to read API_functions.txt. file position: " << ex.what() << std::endl;
            is.close();
            throw std::logic_error("Unable to read API_functions.txt");
        }
        is.close();

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
        database_manager::UserData ud {"", std::move(rd->login), std::move(rd->password)};
        bool add_line_res = uds_->AddLine(ud);
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
        ud = uds_->GetByLoginPassword(std::move(rd->login), std::move(rd->password));
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
        auto user_data = uds_->GetByUuid(*uuid);
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

    tokenm::Token ApiHandler::SenderAuthentication(const HttpRequest& request) {
        auto auth_iter = request.find(http::field::authorization);
        std::optional<tokenm::Token> token = GetTokenFromHeader(auth_iter->value().to_string());
        return *token;
    }
    
} // http_handler