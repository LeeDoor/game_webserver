#pragma once
#include "api_handler.hpp"

namespace http_handler{
    class DebugHandler : public ApiHandler, public std::enable_shared_from_this<DebugHandler> {
    public:
        DebugHandler(HandlerParameters handler_parameters);
        void Init() override;
    private:
        // generate api function set from file
        void ApiFunctionsParse();

        void ApiGetPlayerTokens(SessionData rns);
        void ApiGetUserData(SessionData rns);
        void ApiGetMMQueue(SessionData rns);

        database_manager::IUserDataManager::Ptr udm_;
        token_manager::ITokenManager::Ptr tm_;
        game_manager::IQueueManager::Ptr qm_;
    };
}


