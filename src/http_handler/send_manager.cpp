#include "send_manager.hpp"

#define SEND_ERROR(ERROR_NAME, DESC, STATUS) \
        ResponseBuilder<http::string_body> builder;\
        std::string body = serializer_->SerializeError(ERROR_NAME, DESC);\
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(STATUS).GetProduct());

namespace http_handler{

    SendManager::SendManager(serializer::ISerializer::Ptr ser){
        serializer_ = ser;
    }

    void SendManager::SendSuccess(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeEmpty();
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::ok).GetProduct());
    }
    void SendManager::SendToken(RequestNSender rns, token_manager::Token& token){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeMap({{"token", token}});
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::ok).GetProduct());
    }
    void SendManager::SendUserData(RequestNSender rns, const PublicUserData& puser_data)  {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializePublicUserData(puser_data);
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::ok).GetProduct());
    }

    void SendManager::SendWrongApiFunction(RequestNSender rns) {
        SEND_ERROR("api_error", "wrong api function", status::bad_request)
    }
    void SendManager::SendWrongBodyData(RequestNSender rns) {
        SEND_ERROR("body_data_error", "wrong body data", status::bad_request)
    }
    void SendManager::SendLoginTaken(RequestNSender rns) {
        SEND_ERROR("login_taken", "login is already taken", status::conflict)
    }
    void SendManager::SendWrongLoginOrPassword(RequestNSender rns){
        SEND_ERROR("wrong_login_or_password", "login size >= 3 password size >= 6 with digit(s)", status::bad_request)
    }
    void SendManager::SendNoSuchUser(RequestNSender rns){
        SEND_ERROR("no_such_user", "no user with this login or password", status::bad_request)
    }
    void SendManager::SendUnauthorized(RequestNSender rns) {
        SEND_ERROR("unathorized", "request must be authorized", status::unauthorized)
    }
    void SendManager::SendInvalidToken(RequestNSender rns) {
        SEND_ERROR("invalid_token", "request authorization is invalid", status::unauthorized)
    }
    void SendManager::SendTokenToRemovedPerson(RequestNSender rns) {
        SEND_ERROR("person_removed", "person with this token is unavailable (prob. removed)", status::unauthorized)
    }
    void SendManager::SendCantEnqueue(RequestNSender rns) {
        SEND_ERROR("enqueue_error", "error happened while enqueuing player", status::ok)
    }

        
    void SendManager::HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, RequestNSender rns) {
        switch(status) {
        case ApiStatus::WrongMethod:
            SendWrongMethod(executor, rns);
            break;
        case ApiStatus::Unauthorized:
            SendUnauthorized(rns);
            break;
        case ApiStatus::InvalidToken:
            SendInvalidToken(rns);
            break;
        default:
            SendUndefinedError(executor, rns);
            break;
        }
    }
    
    void SendManager::SendWrongMethod(const ApiFunctionExecutor& executor, RequestNSender rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("wrong_method", "method not allowed");
        const std::vector<http::verb>& methods = executor.GetApiFunction().GetAllowedMethods();
        bool can_head = std::find(methods.begin(), methods.end(), http::verb::head) != methods.end();
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method(), can_head).Status(status::method_not_allowed).Allow(methods).GetProduct());
    }
    void SendManager::SendUndefinedError(const ApiFunctionExecutor& executor, RequestNSender rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("undefined_error", "some weird error happened");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
}