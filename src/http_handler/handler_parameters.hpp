#pragma once
#include "i_serializer.hpp"
#include "i_user_manager.hpp"
#include "i_token_manager.hpp"
#include "i_queue_manager.hpp"
#include "game_manager.hpp"
#include <memory>

namespace http_handler{
    struct HandlerParameters{
        serializer::ISerializer::Ptr serializer;
        user_manager::IUserManager::Ptr user_manager;
        token_manager::ITokenManager::Ptr token_manager;
        game_manager::IQueueManager::Ptr queue_manager;
        game_manager::GameManager::Ptr game_manager;

        fs::path static_path;
    };

}