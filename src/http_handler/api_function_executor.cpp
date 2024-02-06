#include <algorithm>
#include "api_function_executor.hpp"
#include "get_token_from_header.hpp"

namespace http_handler {
    ApiFunctionExecutor::ApiFunctionExecutor(ApiFunction&& api_function, std::optional<token_manager::TokenToUuid::Ptr> ttu):
         api_function_ (std::move(api_function)), ttu_(ttu){}

    ApiStatus ApiFunctionExecutor::Execute(RequestNSender rns) {
        if(!MatchMethod(rns.request.method())){
            return ApiStatus::WrongMethod;
        }
        if(ttu_.has_value()){
            ApiStatus author_result = MatchAuthorization(rns.request);
            if(author_result != ApiStatus::Ok){
                return author_result;
            }
        }
        api_function_(rns);
        return ApiStatus::Ok;
    }
    const ApiFunction& ApiFunctionExecutor::GetApiFunction() const{
        return api_function_;
    }

    bool ApiFunctionExecutor::MatchMethod(const http::verb& verb) {
        const AllowedMethods& methods = api_function_.GetAllowedMethods();
        return std::find(methods.begin(), methods.end(), verb) != methods.end();
    }
    ApiStatus ApiFunctionExecutor::MatchAuthorization(const HttpRequest& request) {
        auto auth_iter = request.find(http::field::authorization);
        if (auth_iter == request.end())
            return ApiStatus::Unauthorized;

        std::optional<tokenm::Token> token = GetTokenFromHeader(auth_iter->value().to_string());
        if(!token)
            return ApiStatus::InvalidToken;

        std::optional<std::string> uuid = ttu_.value()->GetUuidByToken(*token);
        if(!uuid)
            return ApiStatus::InvalidToken;
        return ApiStatus::Ok;
    }

} // http_handler