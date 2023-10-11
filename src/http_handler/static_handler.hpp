#pragma once
#include <filesystem>
#include <memory>
#include "http_types.hpp"
#include "i_serializer.hpp"

namespace http_handler {
    using namespace serializer;

    class StaticHandler {
    public:
        StaticHandler(std::shared_ptr<ISerializer> serializer);

        void HandleFile(fs::path&& path, 
                        fs::path&& root, 
                        StrResponseSender&& string_sender,
                        FileResponseSender&& file_sender);
    private:
        void SendFile(const fs::path& path, FileResponseSender&& file_sender);
        void SendWrongPathError(StrResponseSender&& string_sender);    
        void SendNoAccessError(StrResponseSender&& string_sender);

        bool IsSubdirectory(fs::path& path, fs::path& base);

        std::shared_ptr<ISerializer> serializer_;
    };

} // http_handler


