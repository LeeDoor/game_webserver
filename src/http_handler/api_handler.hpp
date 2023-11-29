#pragma once
#include "api_function_builder.hpp"
#include "i_serializer.hpp"
#include "response_builder.hpp"
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

        //api functions
        void ApiRegister(RequestNSender rns);
        void ApiLogin(RequestNSender rns);

        //api requests
        void SendSuccess(RequestNSender rns);
        void SendToken(RequestNSender rns, token_manager::Token& token);
        
        void SendWrongApiFunction(RequestNSender rns);
        void SendWrongBodyData(RequestNSender rns);
        void SendLoginTaken(RequestNSender rns);
        void SendWrongLoginOrPassword(RequestNSender rns);
        void SendNoSuchUser(RequestNSender rns);

        void HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, RequestNSender rns);
        void SendWrongMethod(const ApiFunctionExecutor& executor, RequestNSender rns);
        void SendUndefinedError(const ApiFunctionExecutor& executor, RequestNSender rns);

        std::map<std::string, ApiFunctionExecutor> request_to_executor_;
        serializer::ISerializer::Ptr serializer_;
        database_manager::IUserDataManager::Ptr uds_;
        token_manager::TokenToUuid::Ptr ttu_;
    };

} // http_handler

