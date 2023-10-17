//
// Created by leedoor on 22.08.23.
//

#include "api_function_executor.hpp"
#include <algorithm>

namespace http_handler {
    ApiFunctionExecutor::ApiFunctionExecutor(ApiFunction&& api_function):
         api_function_ (std::move(api_function)){}

    void ApiFunctionExecutor::Execute(HttpRequest&& request, ResponseSender&& sender) {
        //if(!MatchMethod(request.method())){

        //}
        api_function_(std::move(request), std::move(sender));
    }

    bool ApiFunctionExecutor::MatchMethod(const http::verb& verb) {
        AllowedMethods& methods = api_function_.GetAllowedMethods();
        return std::find(methods.begin(), methods.end(), verb) != methods.end();
    }
} // http_handler