#pragma once
#include "api_function.hpp"
#include <optional>
#include "token_to_uuid.hpp"
#include "i_serializer.hpp"

namespace http_handler {
    enum ApiStatus {
        Ok,
        WrongMethod,
        Unauthorized,
        InvalidToken,
        AdminUnrecognized,
    };

    class ApiFunctionExecutor {
    public:
        ApiFunctionExecutor(ApiFunctionParams&& api_function_params, 
            std::optional<token_manager::TokenToUuid::Ptr> ttu,
            serializer::ISerializer::Ptr serializer_);

        ApiStatus Execute(RequestNSender rns);
        const ApiFunction& GetApiFunction() const;
    private:
        bool MatchMethod(const http::verb& verb);
        ApiStatus MatchAuthorization(const HttpRequest& request);
        bool MatchAdmin(const HttpRequest& request);

        ApiFunction api_function_;
        std::optional<token_manager::TokenToUuid::Ptr> ttu_;
        serializer::ISerializer::Ptr serializer_;
    };

} // http_handler