#pragma once
#include "api_function_executor.hpp"
#include <vector>

namespace http_handler {
    class ApiFunctionBuilder {
    public:
        ApiFunctionBuilder& SetMethods(AllowedMethods&& allowed_methods);
        ApiFunctionBuilder& SetGetHeadMethods();
        ApiFunctionBuilder& SetPostMethods();
        ApiFunctionBuilder& SetExecutionFunction(ExecutorFunction&& function);
        ApiFunctionExecutor GetProduct();

    private:
        AllowedMethods allowed_methods_;
        ExecutorFunction executor_function_;
    };

}