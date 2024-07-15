#pragma once
#include <memory>
#include "http_types.hpp"
#include "handler_parameters.hpp"
#include "send_manager.hpp"

namespace http_handler{

    class GameHandler : public std::enable_shared_from_this<GameHandler>{
    public:
        using Ptr = std::shared_ptr<GameHandler>;

        GameHandler(HandlerParameters handler_parameters);
        void Init();
        void ApiFunctionsParse();

        void Handle(HttpRequest&& request, SessionFunctions&& session_functions);

        //apis

        void ApiEnqueue(SessionData rns);

    private:
        SendManager responser_;
        serializer::ISerializer::Ptr serializer_;
        std::map<std::string, ApiFunctionExecutor> request_to_executor_;

        game_manager::IQueueManager::Ptr iqm_;
        game_manager::GameManager::Ptr gm_;
        token_manager::ITokenManager::Ptr tm_;
    };

}