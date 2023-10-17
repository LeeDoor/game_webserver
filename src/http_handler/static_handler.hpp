#pragma once
#include <filesystem>
#include <memory>
#include "http_types.hpp"
#include "i_serializer.hpp"
#include "response_sender.hpp"

namespace http_handler {
    using namespace serializer;

    class StaticHandler {
    public:
        StaticHandler(std::shared_ptr<ISerializer> serializer);

        void HandleFile(HttpRequest&& request, fs::path&& root, ResponseSender&& sender);
    private:
        void SendFile(fs::path&& path, HttpRequest&& request, FileResponseSender&& file_sender);
        void SendWrongPathError(HttpRequest&& request, StrResponseSender&& string_sender);    
        void SendNoAccessError(HttpRequest&& request, StrResponseSender&& string_sender);

        bool IsSubdirectory(fs::path&& path, fs::path&& base);

        std::shared_ptr<ISerializer> serializer_;
    };

} // http_handler


