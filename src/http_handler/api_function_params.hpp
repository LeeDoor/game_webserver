#pragma once
#include <functional>
namespace http_handler{
	typedef std::function<void(SessionData)> ExecutorFunction;
    typedef std::vector<http::verb> AllowedMethods;

    struct ApiFunctionParams {
        ExecutorFunction executor_function; // function to be executed
        AllowedMethods allowed_methods; // allowed methods for this function
        bool is_debug; // is this method created for debugging
    };
}
    