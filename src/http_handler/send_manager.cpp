#include "send_manager.hpp"
#include "spdlog/spdlog.h"

namespace http_handler{

    SendManager::SendManager(serializer::ISerializer::Ptr ser){
        serializer_ = ser;
    }
    
    void SendManager::Send(SessionData rns, status stat, std::string body) {
        ResponseBuilder<http::string_body> builder;
        spdlog::info("SENT [{}]", static_cast<int>(stat));  
        rns.session_functions.send_string(builder.BodyText(std::move(body), rns.request.method()).Status(stat).GetProduct());
    }
    
    void SendManager::SendSuccess(SessionData rns) {
        Send(rns, status::ok, serializer_->SerializeEmpty());
    }
    void SendManager::SendToken(SessionData rns, token_manager::Token& token){
        Send(rns, status::ok, serializer_->SerializeMap({{"token", token}}));
    }
    void SendManager::SendUserData(SessionData rns, const PublicUserData& puser_data)  {
        Send(rns, status::ok, serializer_->SerializePublicUserData(puser_data));
    }
    void SendManager::SendHiddenUserData(SessionData rns, const dm::UserData& user_data) {
        Send(rns, status::ok, serializer_->SerializeUserData(user_data));
    }

    void SendManager::SendWrongApiFunction(SessionData rns) {
        Send(rns, status::bad_request, serializer_->SerializeError("api_error", "wrong api function"));
    }
    void SendManager::SendWrongBodyData(SessionData rns) {
        Send(rns, status::bad_request, serializer_->SerializeError("body_data_error", "wrong body data"));
    }
    void SendManager::SendLoginTaken(SessionData rns) {
        Send(rns, status::conflict, serializer_->SerializeError("login_taken", "login is already taken"));
    }
    void SendManager::SendWrongLoginOrPassword(SessionData rns){
        Send(rns, status::bad_request, serializer_->SerializeError("wrong_login_or_password", "login size >= 3 password size >= 6 with digit(s)"));
    }
    void SendManager::SendNoSuchUser(SessionData rns){
        Send(rns, status::bad_request, serializer_->SerializeError("no_such_user", "no user with this login or password"));
    }
    void SendManager::SendUnauthorized(SessionData rns) {
        Send(rns, status::unauthorized, serializer_->SerializeError("unathorized", "request must be authorized"));
    }
    void SendManager::SendInvalidToken(SessionData rns) {
        Send(rns, status::unauthorized, serializer_->SerializeError("invalid_token", "request authorization is invalid"));
    }
    void SendManager::SendAdminUnrecognized(SessionData rns){
        Send(rns, status::unauthorized, serializer_->SerializeError("invalid_admin", "the administrator password is missing or incorrect"));
    }
    void SendManager::SendTokenToRemovedPerson(SessionData rns) {
        Send(rns, status::unauthorized, serializer_->SerializeError("person_removed", "person with this token is unavailable (prob. removed)"));
    }
    void SendManager::SendCantEnqueue(SessionData rns) {
        Send(rns, status::ok, serializer_->SerializeError("enqueue_error", "error happened while enqueuing player (already in queue)"));
    }
    void SendManager::SendCantLogin(SessionData rns){
        Send(rns, status::service_unavailable, serializer_->SerializeError("login_error", "unable to add token to database."));
    }

    void SendManager::SendWrongUrlParameters(SessionData rns){
        Send(rns, status::unprocessable_entity, serializer_->SerializeError("url_parameters_error", "this api function requires url parameters"));
    }

    void SendManager::HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, SessionData rns) {
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
        case ApiStatus::AdminUnrecognized:
            SendAdminUnrecognized(rns);
            break;
        default:
            SendUndefinedError(executor, rns);
            break;
        }
    }
    
    void SendManager::SendWrongMethod(const ApiFunctionExecutor& executor, SessionData rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("wrong_method", "method not allowed");
        const std::vector<http::verb>& methods = executor.GetApiFunction().GetAllowedMethods();
        bool can_head = std::find(methods.begin(), methods.end(), http::verb::head) != methods.end();
        rns.session_functions.send_string(builder.BodyText(std::move(body), rns.request.method(), can_head).Status(status::method_not_allowed).Allow(methods).GetProduct());
    }
    void SendManager::SendUndefinedError(const ApiFunctionExecutor& executor, SessionData rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("undefined_error", "some weird error happened");
        rns.session_functions.send_string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
}