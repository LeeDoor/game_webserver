#pragma once
#include "api_function_builder.hpp"
#include <map>

namespace http_handler {

    class ApiHandler {
    public:
        void HandleApiFunction(HttpRequest&& request, StrResponseSender&& sender);

    private:
        void BuildTargetsMap();

        std::map<std::string, ApiFunctionExecutor> request_to_executor_;
    };

} // http_handler

