#include "send_manager.hpp"

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
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("api_error", "wrong api function");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
    void SendManager::SendWrongBodyData(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("body_data_error", "wrong body data");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
    void SendManager::SendLoginTaken(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("login_taken", "login is already taken");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::conflict).GetProduct());
    }
    void SendManager::SendWrongLoginOrPassword(RequestNSender rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("wrong_login_or_password", "login size >= 3 password size >= 6 with digit(s)");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
    void SendManager::SendNoSuchUser(RequestNSender rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("no_such_user", "no user with this login or password");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
    void SendManager::SendUnauthorized(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("unathorized", "request must be authorized");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::unauthorized).GetProduct());
    }
    void SendManager::SendInvalidToken(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("invalid_token", "request authorization is invalid");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::unauthorized).GetProduct());
    }
    void SendManager::SendTokenToRemovedPerson(RequestNSender rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("person_removed", "person with this token is unavailable (prob. removed)");
        rns.sender.string(builder.BodyText(std::move(body), rns.request.method()).Status(status::unauthorized).GetProduct());
    }

        
    void SendManager::HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, RequestNSender rns) {
        switch(status) {
        case ApiStatus::WrongMethod:
            SendWrongMethod(executor, rns);
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