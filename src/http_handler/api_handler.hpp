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
        void Handle(HttpRequest&& request, ResponseSender&& sender);

    private:
        // generate api function set from file
        void ApiFunctionsParse();
        
        // api functions
        void ApiRegister(RequestNSender rns);
        void ApiLogin(RequestNSender rns);
        void ApiGetProfileData(RequestNSender rns);
        void ApiEnqueue(RequestNSender rns);

        // api debug functions
        void ApiGetPlayerTokens(RequestNSender rns);
        void ApiGetUserData(RequestNSender rns);
        void ApiGetMMQueue(RequestNSender rns);

        // identify the user by the token
        tokenm::Token SenderAuthentication(const HttpRequest& request);

        // get url parameters
        std::map<std::string, std::string> ParseUrlParameters(const HttpRequest& request);
        std::pair<std::string, std::string> ParseUrlPair(std::string&& pair);

        SendManager responser_;
        std::map<std::string, ApiFunctionExecutor> request_to_executor_;
        serializer::ISerializer::Ptr serializer_;
        database_manager::IUserDataManager::Ptr udm_;
        token_manager::ITokenManager::Ptr tm_;
        matchmaking_system::IQueueManager::Ptr iqm_;
        matchmaking_system::MatchmakingBalancer::Ptr mb_;
        std::string api_path_;
    };

} // http_handler

