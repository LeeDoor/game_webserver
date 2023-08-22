#pragma once
#include <filesystem>

namespace http_handler {

    class StaticHandler {
    public:
        void SendStaticFile(const std::filesystem::path& path);
    };

} // http_handler


