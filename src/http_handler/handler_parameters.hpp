#pragma once
#include "i_serializer.hpp"
#include "i_user_data_manager.hpp"
#include "token_to_uuid.hpp"
#include "mm_queue.hpp"
#include <memory>

namespace http_handler{
    struct HandlerParameters{
        serializer::ISerializer::Ptr serializer;
        database_manager::IUserDataManager::Ptr user_data_manager;
        token_manager::TokenToUuid::Ptr token_to_uuid;
        matchmaking_system::MMQueue::Ptr mm_queue;
    };

}