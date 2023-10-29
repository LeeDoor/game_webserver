#pragma once
#include "api_function.hpp"

namespace http_handler {

    enum ApiStatus {
        Ok,
        WrongMethod
    };

    class ApiFunctionExecutor {
    public:
        ApiFunctionExecutor(ApiFunction&& api_function);

        ApiStatus Execute(RequestNSender rns);
        const ApiFunction& GetApiFunction() const;
    private:
        bool MatchMethod(const http::verb& verb);

        ApiFunction api_function_;
    };

} // http_handler