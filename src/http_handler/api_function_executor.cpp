#include <algorithm>
#include "api_function_executor.hpp"
#include "get_token_from_header.hpp"
#include "registration_data.hpp"

namespace http_handler {
    ApiFunctionExecutor::ApiFunctionExecutor(ApiFunctionParams&& api_function_params, 
        std::optional<token_manager::ITokenManager::Ptr> tm,
        serializer::ISerializer::Ptr serializer):
        api_function_ (std::move(api_function_params)), tm_(tm), serializer_(serializer){}

    ApiStatus ApiFunctionExecutor::Execute(RequestNSender rns) {
        if(!MatchMethod(rns.request.method())){
            return ApiStatus::WrongMethod;
        }
        if(tm_.has_value()){
            ApiStatus author_result = MatchAuthorization(rns.request);
            if(author_result != ApiStatus::Ok){
                return author_result;
            }
        }
        if (api_function_.IsDebug() && !MatchAdmin(rns.request)){
            return ApiStatus::AdminUnrecognized;
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
        std::optional<dm::Uuid> uuid = tm_.value()->GetUuidByToken(*token);
        if(!uuid)
            return ApiStatus::InvalidToken;
        return ApiStatus::Ok;
    }
    bool ApiFunctionExecutor::MatchAdmin(const HttpRequest& request) {
        std::optional<RegistrationData> rd = 
            serializer_->DeserializeRegData(request.body());
        if (!rd.has_value())
            return false;
        return rd->login == "leedoor" && rd->password == "123qwe123";
    }

} // http_handler