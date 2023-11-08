#pragma once
#include <filesystem>
#include <memory>
#include "http_types.hpp"
#include "i_serializer.hpp"
#include "handler_parameters.hpp"

namespace http_handler {
    using namespace serializer;

    class StaticHandler {
    public:
        StaticHandler(HandlerParameters handler_parameters);

        void HandleFile(HttpRequest&& request, fs::path&& root, ResponseSender&& sender);
    private:
        void SendFile(fs::path&& path, RequestNSender rns);
        void SendWrongPathError(RequestNSender rns);    
        void SendNoAccessError(RequestNSender rns);

        bool IsSubdirectory(fs::path&& path, fs::path&& base);

        std::shared_ptr<ISerializer> serializer_;
    };

} // http_handler


