#pragma once
#include "api_function_executor.hpp"
#include <vector>

namespace http_handler {
    class ApiFunctionBuilder {
    public:
        //sets alllowed methods to given array
        ApiFunctionBuilder& Methods(AllowedMethods&& allowed_methods);
        //sets allowed methods to get and head
        ApiFunctionBuilder& GetHead();
        //sets allowed method to post
        ApiFunctionBuilder& Post();
        //sets execution function to given
        ApiFunctionBuilder& ExecFunc(ExecutorFunction&& function);
        //makes and returns product
        ApiFunctionExecutor GetProduct();

    private:
        AllowedMethods allowed_methods_;
        ExecutorFunction executor_function_;
    };

}