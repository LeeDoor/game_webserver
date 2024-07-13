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
        using Ptr = std::shared_ptr<StaticHandler>;

        StaticHandler(HandlerParameters handler_parameters);

        void Handle(HttpRequest&& request, SessionFunctions&& session_functions);
    private:
        void SendFile(fs::path&& path, SessionData rns);
        void SendWrongPathError(SessionData rns);    
        void SendNoAccessError(SessionData rns);

        bool IsSubdirectory(fs::path&& path);

        std::shared_ptr<ISerializer> serializer_;
        fs::path static_path_;
    };

} // http_handler


