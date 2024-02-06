#pragma once
#include "api_function_builder.hpp"
#include "i_serializer.hpp"
#include "response_builder.hpp"
#include "send_manager.hpp"
#include "handler_parameters.hpp"
#include <map>

namespace http_handler {
    class ApiHandler :  public std::enable_shared_from_this<ApiHandler> {
    public:
        ApiHandler(HandlerParameters handler_parameters);
        void Init();
        void HandleApiFunction(HttpRequest&& request, ResponseSender&& sender);

    private:
        void BuildTargetsMap();
        void ApiRegister(RequestNSender rns);
        void ApiLogin(RequestNSender rns);
        void ApiGetProfileData(RequestNSender rns);
        void ApiEnqueue(RequestNSender rns);

        std::map<std::string, ApiFunctionExecutor> request_to_executor_;
        serializer::ISerializer::Ptr serializer_;
        SendManager responser_;
        database_manager::IUserDataManager::Ptr uds_;
        token_manager::TokenToUuid::Ptr ttu_;
    };

} // http_handler

