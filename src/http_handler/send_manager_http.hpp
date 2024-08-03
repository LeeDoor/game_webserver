#pragma once
#include "http_types.hpp"
#include "api_function_executor.hpp"
namespace http_handler{
    void HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, SessionData rns);
    void SendWrongMethod(const ApiFunctionExecutor& executor, SessionData rns);
    void SendUndefinedError(const ApiFunctionExecutor& executor, SessionData rns);
}