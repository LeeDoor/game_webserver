#pragma once
#include <filesystem>
#include "http_types.hpp"

namespace http_handler {

    class StaticHandler {
    public:
        void HandleFile(fs::path&& path, 
                        fs::path&& root, 
                        StrResponseSender&& string_sender,
                        FileResponseSender&& file_sender);
    private:
        void SendFile(const fs::path& path, FileResponseSender&& file_sender);
        void SendWrongPathError(StrResponseSender&& string_sender);    
        void SendNoAccessError(StrResponseSender&& string_sender);

        bool IsSubdirectory(fs::path& path, fs::path& base);
    };

} // http_handler


