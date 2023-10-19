//
// Created by leedoor on 22.08.23.
//

#include "api_function_executor.hpp"
#include <algorithm>

namespace http_handler {
    ApiFunctionExecutor::ApiFunctionExecutor(ApiFunction&& api_function):
         api_function_ (std::move(api_function)){}

    ApiStatus ApiFunctionExecutor::Execute(const HttpRequest& request, const ResponseSender& sender) {
        if(!MatchMethod(request.method())){
            return ApiStatus::WrongMethod;
        }
        api_function_(request, sender);
        return ApiStatus::Ok;
    }
    const ApiFunction& ApiFunctionExecutor::GetApiFunction() const{
        return api_function_;
    }

    bool ApiFunctionExecutor::MatchMethod(const http::verb& verb) {
        const AllowedMethods& methods = api_function_.GetAllowedMethods();
        return std::find(methods.begin(), methods.end(), verb) != methods.end();
    }
} // http_handler