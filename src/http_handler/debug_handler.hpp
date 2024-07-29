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
        void ApiGetUser(SessionData rns);
        void ApiGetMMQueue(SessionData rns);

        user_manager::IUserManager::Ptr dm_;
        token_manager::ITokenManager::Ptr tm_;
        game_manager::IQueueManager::Ptr qm_;
    };
}


