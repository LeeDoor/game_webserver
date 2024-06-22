#include "api_function.hpp"

namespace http_handler {
    ApiFunction::ApiFunction(ApiFunctionParams&& params):
        executor_function_(std::move(params.executor_function)),
        allowed_methods_(std::move(params.allowed_methods)),
        is_debug_(params.is_debug){}

    const AllowedMethods& ApiFunction::GetAllowedMethods() const{
        return allowed_methods_;
    }
    bool ApiFunction::IsDebug() const {
        return is_debug_;
    }
    void ApiFunction::operator()(RequestNSender rns) {
        executor_function_(rns);
    }
} // http_handler