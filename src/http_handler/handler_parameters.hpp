#pragma once
#include "i_user_manager.hpp"
#include "i_token_manager.hpp"
#include "i_queue_manager.hpp"
#include "i_session_manager.hpp"
#include "game_manager.hpp"
#include <memory>

namespace http_handler{
    struct HandlerParameters{
        um::IUserManager::Ptr user_manager;
        token_manager::ITokenManager::Ptr token_manager;
        gm::IQueueManager::Ptr queue_manager;
        gm::GameManager::Ptr game_manager;
        sm::ISessionManager::Ptr session_manager;

        fs::path static_path;
    };

}