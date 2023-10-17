//
// Created by leedoor on 22.08.23.
//

#include "api_function_builder.hpp"

namespace http_handler{
    ApiFunctionBuilder& ApiFunctionBuilder::SetMethods(AllowedMethods&& allowed_methods) {
        allowed_methods = std::move(allowed_methods);
        return *this;
    }
    ApiFunctionBuilder& ApiFunctionBuilder::SetGetHeadMethods() {
        allowed_methods_ = {http::verb::head, http::verb::get};
        return *this;
    }
    ApiFunctionBuilder& ApiFunctionBuilder::SetPostMethods() {
        allowed_methods_ = {http::verb::post};
        return *this;
    }
    ApiFunctionBuilder& ApiFunctionBuilder::SetExecutionFunction(ExecutorFunction&& function) {
        executor_function_ = std::move(function);
        return *this;
    }
    ApiFunctionExecutor ApiFunctionBuilder::GetProduct() {
        //creating api function executor 
        return {{std::move(executor_function_), std::move(allowed_methods_)}};
    }
}