#include "api_function_builder.hpp"

namespace http_handler{
    ApiFunctionBuilder::ApiFunctionBuilder(serializer::ISerializer::Ptr serializer):
        serializer_(serializer){}

    ApiFunctionBuilder& ApiFunctionBuilder::Methods(AllowedMethods&& allowed_methods) {
        allowed_methods_ = std::move(allowed_methods);
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
    ApiFunctionBuilder& ApiFunctionBuilder::NeedAuthor(token_manager::TokenToUuid::Ptr ttu){
        ttu_ = ttu;
        return *this;
    }
    ApiFunctionBuilder& ApiFunctionBuilder::ForDebug(){
        is_debug_ = true;
        return *this;
    }
    ApiFunctionExecutor ApiFunctionBuilder::GetProduct() {
        //creating api function executor 
        ApiFunctionExecutor afe = 
        {
            {
                std::move(executor_function_), 
                std::move(allowed_methods_),
                is_debug_
            }, 
            ttu_,
            serializer_
        };
        ttu_ = std::nullopt;
        allowed_methods_.clear();
        executor_function_ = {};
        is_debug_ = false;
        return afe;
    }
}