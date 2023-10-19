#pragma once
#include "api_function_executor.hpp"
#include <vector>

namespace http_handler {
    class ApiFunctionBuilder {
    public:
        ApiFunctionBuilder& Methods(AllowedMethods&& allowed_methods);
        ApiFunctionBuilder& GetHead();
        ApiFunctionBuilder& Post();
        ApiFunctionBuilder& ExecFunc(ExecutorFunction&& function);
        ApiFunctionExecutor GetProduct();

    private:
        AllowedMethods allowed_methods_;
        ExecutorFunction executor_function_;
    };

}