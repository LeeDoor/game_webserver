#include <algorithm>
#include "api_function_executor.hpp"
#include "registration_data.hpp"
#include "serializer_http.hpp"

namespace http_handler {
    ApiFunctionExecutor::ApiFunctionExecutor(ApiFunctionParams&& api_function_params, 
        std::optional<token_manager::ITokenManager::Ptr> tm):
        api_function_ (std::move(api_function_params)), tm_(tm){}

    ApiStatus ApiFunctionExecutor::Execute(SessionData&& rns) {
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
        api_function_(std::move(rns), rd_);
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
        std::optional<um::Uuid> uuid = tm_.value()->GetUuidByToken(*token);
        if(!uuid)
            return ApiStatus::InvalidToken;
        rd_.uuid = *uuid;
        return ApiStatus::Ok;
    }
    bool ApiFunctionExecutor::MatchAdmin(const HttpRequest& request) {
        std::optional<RegistrationData> rd = 
            serializer::DeserializeRegData(request.body());
        if (!rd.has_value())
            return false;
        return rd->login == "leedoor" && rd->password == "123qwe123";
    }

    std::optional<tokenm::Token> ApiFunctionExecutor::GetTokenFromHeader(const std::string& header){
        if(header.substr(0, 7) == "Bearer "){
            tokenm::Token token = header.substr(7);
            if(token.size() == 32)
                return token;
        }
        return std::nullopt;
    }
} // http_handler