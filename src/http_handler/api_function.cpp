//
// Created by leedoor on 22.08.23.
//

#include "api_function.hpp"

namespace http_handler {
    ApiFunction::ApiFunction(ExecutorFunction&& executor_function, AllowedMethods&& allowed_methods):
        executor_function_(std::move(executor_function)), allowed_methods_(std::move(allowed_methods)){}

    const AllowedMethods& ApiFunction::GetAllowedMethods() const{
        return allowed_methods_;
    }

    void ApiFunction::operator()(const HttpRequest& request, const ResponseSender& sender) {
        executor_function_(request, sender);
    }
} // http_handler