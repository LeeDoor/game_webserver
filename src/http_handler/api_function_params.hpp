#pragma once
#include <functional>
#include <optional>
#include "user.hpp"
#include "http_types.hpp"
namespace http_handler{
    struct RequestData{
        // uuid of requestor (empty string if no requestor provided)
        um::Uuid uuid;
    };

	typedef std::function<void(SessionData&&, const RequestData&)> ExecutorFunction;
    typedef std::vector<http::verb> AllowedMethods;

    struct ApiFunctionParams {
        ExecutorFunction executor_function; // function to be executed
        AllowedMethods allowed_methods; // allowed methods for this function
        bool is_debug; // is this method created for debugging
    };
}
    