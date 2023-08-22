#pragma once
#include "api_function.hpp"

namespace http_handler {

    class ApiFunctionExecutor {
    public:
        ApiFunctionExecutor(ApiFunction&& api_function);

        void Execute(HttpRequest&& http_request, ResponseSender&& response_sender);

    private:
        bool MatchMethod(const http::verb& verb);

        ApiFunction api_function_;
    };

} // http_handler
