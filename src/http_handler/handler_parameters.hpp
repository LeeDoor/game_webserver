#pragma once
#include "i_serializer.hpp"
#include <memory>

namespace http_handler{
    struct HandlerParameters{
        std::shared_ptr<serializer::ISerializer> serializer;
    };

}