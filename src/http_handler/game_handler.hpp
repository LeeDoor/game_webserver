#pragma once
#include <memory>
#include "http_types.hpp"
#include "api_handler.hpp"
#include "i_session_manager.hpp"

namespace http_handler{

    class GameHandler : public ApiHandler, public std::enable_shared_from_this<GameHandler>{
    public:
        using Ptr = std::shared_ptr<GameHandler>;

        GameHandler(HandlerParameters handler_parameters);
        void Init() override;
        void ApiFunctionsParse();

        //apis
        void ApiEnqueue(SessionData rns);
        void ApiWaitForOpponent(SessionData&& rns);
        void ApiSessionState(SessionData rns);
        void ApiSessionStateChange(SessionData&& rns);
        void ApiMove(SessionData rns);
        void ApiResign(SessionData rns);

    private:
        std::optional<gm::SessionId> ParseUrlSessionId(const HttpRequest& request);
        bool DefineSessionState(SessionData rns, std::optional<gm::SessionId>& sid);

        game_manager::IQueueManager::Ptr iqm_;
        game_manager::GameManager::Ptr gm_;
        token_manager::ITokenManager::Ptr tm_;
        sm::ISessionManager::Ptr sm_;
    };

}