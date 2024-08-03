#pragma once
#include "http_types.hpp"
#include "api_function_params.hpp"

namespace http_handler {
    class ApiFunction {
    public:
        ApiFunction(ApiFunctionParams&& params);

        const AllowedMethods& GetAllowedMethods() const;
        bool IsDebug() const;
        void operator()(SessionData&& rns, const RequestData& rd);

    private:
        ExecutorFunction executor_function_;
        AllowedMethods allowed_methods_;
        bool is_debug_;
    };

} // http_handler

