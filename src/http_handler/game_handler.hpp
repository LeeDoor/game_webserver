#pragma once
#include <memory>
#include "http_types.hpp"
#include "handler_parameters.hpp"

namespace http_handler{

    class GameHandler{
    public:
        using Ptr = std::shared_ptr<GameHandler>;

        GameHandler(HandlerParameters handler_parameters);

        void Handle(HttpRequest&& request, SessionFunctions&& session_functions);
    };

}