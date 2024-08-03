#pragma once
#include <filesystem>
#include <memory>
#include "http_types.hpp"
#include "api_handler.hpp"

namespace http_handler {
    class StaticHandler : public ApiHandler{
    public:
        using Ptr = std::shared_ptr<StaticHandler>;

        StaticHandler(HandlerParameters handler_parameters);
        void Init() override;
        void Handle(HttpRequest&& request, SessionFunctions&& session_functions);
    private:
        void SendFile(fs::path&& path, SessionData rns);
        void SendWrongPathError(SessionData rns);    
        void SendNoAccessError(SessionData rns);

        bool IsSubdirectory(fs::path&& path);

        fs::path static_path_;
    };

} // http_handler


