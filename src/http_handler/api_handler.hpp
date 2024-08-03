#pragma once
#include <map>
#include <memory>
#include "http_types.hpp"
#include "api_function_executor.hpp"
#include "handler_parameters.hpp"

namespace http_handler{
    class ApiHandler {
    public:
        using Ptr = std::shared_ptr<ApiHandler>;

        virtual void Init() = 0;
        virtual void Handle(HttpRequest&& request, SessionFunctions&& session_functions);
    protected:
        // get url parameters
        std::map<std::string, std::string> ParseUrlParameters(const HttpRequest& request);
        std::pair<std::string, std::string> ParseUrlPair(std::string&& pair);

        std::map<std::string, ApiFunctionExecutor> request_to_executor_;

    };
}