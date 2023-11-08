#pragma once
#include "i_serializer.hpp"
#include "i_user_data_manager.hpp"
#include <memory>

namespace http_handler{
    struct HandlerParameters{
        serializer::ISerializer::Ptr serializer;
        database_manager::IUserDataManager::Ptr user_data_manager;
    };

}