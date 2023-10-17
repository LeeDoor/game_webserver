#pragma once
#include "http_types.hpp"

namespace http_handler{
    struct ResponseSender{
        StrResponseSender string;
        FileResponseSender file;
    }; 
}