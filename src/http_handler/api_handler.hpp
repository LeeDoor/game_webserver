#pragma once
#include "i_serializer.hpp"
#include "response_builder.hpp"
#include "send_manager.hpp"
#include "handler_parameters.hpp"
#include <map>

namespace http_handler {
    class ApiHandler :  public std::enable_shared_from_this<ApiHandler> {
    public:
        using Ptr = std::shared_ptr<ApiHandler>;

        ApiHandler(HandlerParameters handler_parameters);
        void Init();
        void Handle(HttpRequest&& request, SessionFunctions&& session_functions);

    private:
        // generate api function set from file
        void ApiFunctionsParse();
        
        // api functions
        void ApiRegister(SessionData rns);
        void ApiLogin(SessionData rns);
        void ApiGetProfileData(SessionData rns);
        void ApiEnqueue(SessionData rns);

        // api debug functions
        void ApiGetPlayerTokens(SessionData rns);
        void ApiGetUserData(SessionData rns);
        void ApiGetMMQueue(SessionData rns);

        // identify the user by the token
        tokenm::Token SenderAuthentication(const HttpRequest& request);

        // get url parameters
        std::map<std::string, std::string> ParseUrlParameters(const HttpRequest& request);
        std::pair<std::string, std::string> ParseUrlPair(std::string&& pair);

        SendManager responser_;
        serializer::ISerializer::Ptr serializer_;
        std::map<std::string, ApiFunctionExecutor> request_to_executor_;
        
        database_manager::IUserDataManager::Ptr udm_;
        token_manager::ITokenManager::Ptr tm_;
        game_manager::IQueueManager::Ptr iqm_;
    };

} // http_handler

