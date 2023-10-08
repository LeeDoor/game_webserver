#pragma once
#include <filesystem>
#include "http_types.hpp"

namespace http_handler {

    class StaticHandler {
    public:
        void HandleFile(fs::path&& path, 
                        fs::path&& root, 
                        StrResponseSender&& stringSender,
                        FileResponseSender&& fileSender);
    private:
        void SendFile(const fs::path& path, FileResponseSender&& sender);
        void SendWrongPathError(StrResponseSender&& sender);    
        void SendNoAccessError(StrResponseSender&& sender);

        bool IsSubdirectory(fs::path& path, fs::path& base);
    };

} // http_handler


