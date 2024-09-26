#pragma once
#include <memory>
#include "http_types.hpp"
#include "api_handler.hpp"
#include "i_session_manager.hpp"
#include "i_user_manager.hpp"

namespace http_handler{

    class GameHandler : public ApiHandler, public std::enable_shared_from_this<GameHandler>{
    public:
        using Ptr = std::shared_ptr<GameHandler>;

        GameHandler(HandlerParameters handler_parameters);
        void Init() override;
        void ApiFunctionsParse();

        //apis
        void ApiEnqueue(SessionData&& rns, const RequestData& rd);
        void ApiWaitForOpponent(SessionData&& rns, const RequestData& rd);
        void ApiSessionState(SessionData&& rns, const RequestData& rd);
        void ApiSessionStateChange(SessionData&& rns, const RequestData& rd);
        void ApiMove(SessionData&& rns, const RequestData& rd);

    private:
        std::optional<gm::SessionId> ParseUrlSessionId(const HttpRequest& request);
        void DefineSessionState(SessionData rns, const gm::SessionId& sid);

        game_manager::GameManager::Ptr gm_;
        um::IUserManager::Ptr um_;
        game_manager::IQueueManager::Ptr iqm_;
        token_manager::ITokenManager::Ptr tm_;
        sm::ISessionManager::Ptr sm_;
    };

}