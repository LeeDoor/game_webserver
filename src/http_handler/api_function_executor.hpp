#pragma once
#include "api_function.hpp"
#include <optional>
#include "i_token_manager.hpp"
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
            std::optional<token_manager::ITokenManager::Ptr> tm);

        ApiStatus Execute(SessionData&& rns);
        const ApiFunction& GetApiFunction() const;
    private:
        bool MatchMethod(const http::verb& verb);
        ApiStatus MatchAuthorization(const HttpRequest& request);
        bool MatchAdmin(const HttpRequest& request);

        ApiFunction api_function_;
        std::optional<token_manager::ITokenManager::Ptr> tm_;
    };

} // http_handler