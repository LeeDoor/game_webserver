#pragma once
#include "api_function.hpp"
#include <optional>
#include "token_to_uuid.hpp"

namespace http_handler {
    enum ApiStatus {
        Ok,
        WrongMethod,
        Unauthorized,
        InvalidToken,
    };

    class ApiFunctionExecutor {
    public:
        ApiFunctionExecutor(ApiFunction&& api_function, std::optional<token_manager::TokenToUuid::Ptr> ttu);

        ApiStatus Execute(RequestNSender rns);
        const ApiFunction& GetApiFunction() const;
    private:
        bool MatchMethod(const http::verb& verb);
        ApiStatus MatchAuthorization(const HttpRequest& request);

        ApiFunction api_function_;
        std::optional<token_manager::TokenToUuid::Ptr> ttu_;
    };

} // http_handler