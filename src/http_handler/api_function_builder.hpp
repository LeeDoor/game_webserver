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
        //sets authorization requirement
        ApiFunctionBuilder& NeedAuthor(token_manager::TokenToUuid::Ptr ttu);
        //makes and returns product
        ApiFunctionExecutor GetProduct();

    private:
        AllowedMethods allowed_methods_;
        ExecutorFunction executor_function_;
        //std::nullopt if authorization is not required
        std::optional<token_manager::TokenToUuid::Ptr> ttu_ = std::nullopt;
    };

}