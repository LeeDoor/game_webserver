//
// Created by leedoor on 22.08.23.
//

#include "api_function_builder.hpp"

namespace http_handler{
    ApiFunctionBuilder& ApiFunctionBuilder::Methods(AllowedMethods&& allowed_methods) {
        allowed_methods = std::move(allowed_methods);
        return *this;
    }
    ApiFunctionBuilder& ApiFunctionBuilder::GetHead() {
        allowed_methods_ = {http::verb::head, http::verb::get};
        return *this;
    }
    ApiFunctionBuilder& ApiFunctionBuilder::Post() {
        allowed_methods_ = {http::verb::post};
        return *this;
    }
    ApiFunctionBuilder& ApiFunctionBuilder::ExecFunc(ExecutorFunction&& function) {
        executor_function_ = std::move(function);
        return *this;
    }
    ApiFunctionExecutor ApiFunctionBuilder::GetProduct() {
        //creating api function executor 
        return {{std::move(executor_function_), std::move(allowed_methods_)}};
    }
}