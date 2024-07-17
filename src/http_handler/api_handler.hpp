#pragma once
#include "i_serializer.hpp"
#include "send_manager.hpp"
#include "handler_parameters.hpp"
#include <map>
namespace http_handler{
    class ApiHandler {
    public:
        using Ptr = std::shared_ptr<ApiHandler>;

        ApiHandler(HandlerParameters handler_parameters);

        virtual void Init() = 0;
        virtual void Handle(HttpRequest&& request, SessionFunctions&& session_functions);
    protected:
        // identify the user by the token
        tokenm::Token SenderAuthentication(const HttpRequest& request);

        // get url parameters
        std::map<std::string, std::string> ParseUrlParameters(const HttpRequest& request);
        std::pair<std::string, std::string> ParseUrlPair(std::string&& pair);

        SendManager responser_;
        serializer::ISerializer::Ptr serializer_;
        std::map<std::string, ApiFunctionExecutor> request_to_executor_;

    };
}