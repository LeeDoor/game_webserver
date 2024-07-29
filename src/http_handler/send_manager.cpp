#include "send_manager.hpp"
#include "spdlog/spdlog.h"

namespace http_handler{

    SendManager::SendManager(serializer::ISerializer::Ptr ser){
        serializer_ = ser;
    }
    
    void SendManager::Send(SessionData rns, status stat, std::string body) const {
        ResponseBuilder<http::string_body> builder;
        spdlog::info("SENT [{}] to {}", static_cast<int>(stat), rns.request.target().to_string());  
        StringResponse response = builder.BodyText(std::move(body), rns.request.method()).Status(stat).GetProduct();
        rns.session_functions.send_string(std::move(response));
    }
    
    void SendManager::SendSuccess(SessionData rns) const {
        Send(rns, status::ok, serializer_->SerializeEmpty());
    }
    void SendManager::SendToken(SessionData rns, token_manager::Token& token)const {
        Send(rns, status::ok, serializer_->SerializeMap({{"token", token}}));
    }
    void SendManager::SendUser(SessionData rns, const PublicUser& puser)  const {
        Send(rns, status::ok, serializer_->Serialize(puser));
    }
    void SendManager::SendHiddenUser(SessionData rns, const um::User& user) const {
        Send(rns, status::ok, serializer_->Serialize(user));
    }
    void SendManager::SendSessionId(SessionData rns, const std::string& session_id) const {
        Send(rns, status::ok, serializer_->SerializeMap({{"sessionId", session_id}}));
    }
    void SendManager::SendGameState(SessionData rns, const gm::State& state) const {
        Send(rns, status::ok, serializer_->Serialize(state));
    }

    void SendManager::SendWrongApiFunction(SessionData rns) const {
        Send(rns, status::bad_request, serializer_->SerializeError("api_error", "wrong api function"));
    }
    void SendManager::SendWrongBodyData(SessionData rns) const {
        Send(rns, status::bad_request, serializer_->SerializeError("body_data_error", "wrong body data"));
    }
    void SendManager::SendLoginTaken(SessionData rns) const {
        Send(rns, status::conflict, serializer_->SerializeError("login_taken", "login is already taken"));
    }
    void SendManager::SendWrongLoginOrPassword(SessionData rns)const {
        Send(rns, status::bad_request, serializer_->SerializeError("wrong_login_or_password", "login size >= 3 password size >= 6 with digit(s)"));
    }
    void SendManager::SendNoSuchUser(SessionData rns)const {
        Send(rns, status::bad_request, serializer_->SerializeError("no_such_user", "no user with this login or password"));
    }
    void SendManager::SendUnauthorized(SessionData rns) const {
        Send(rns, status::unauthorized, serializer_->SerializeError("unathorized", "request must be authorized"));
    }
    void SendManager::SendInvalidToken(SessionData rns) const {
        Send(rns, status::unauthorized, serializer_->SerializeError("invalid_token", "request authorization is invalid"));
    }
    void SendManager::SendAdminUnrecognized(SessionData rns)const {
        Send(rns, status::unauthorized, serializer_->SerializeError("invalid_admin", "the administrator password is missing or incorrect"));
    }
    void SendManager::SendTokenToRemovedPerson(SessionData rns) const {
        Send(rns, status::unauthorized, serializer_->SerializeError("person_removed", "person with this token is unavailable (prob. removed)"));
    }
    void SendManager::SendCantEnqueue(SessionData rns) const {
        Send(rns, status::ok, serializer_->SerializeError("enqueue_error", "error happened while enqueuing player (already in queue)"));
    }
    void SendManager::SendInTheMatch(SessionData rns) const {
        Send(rns, status::bad_request, serializer_->SerializeError("in_the_match", "error happened while enqueuing player (already in the match)"));
    }
    void SendManager::SendCantLogin(SessionData rns)const {
        Send(rns, status::service_unavailable, serializer_->SerializeError("login_error", "unable to add token to database."));
    }
    void SendManager::SendPollClosed(SessionData rns, const std::string& description) const {
        Send(rns, status::conflict, serializer_->SerializeError("poll_closed", description));
    }
    void SendManager::SendWrongSessionId(SessionData rns) const{
        Send(rns, status::bad_request, serializer_->SerializeError("wrong_sessionId", "no session with such sessionId"));
    }
    void SendManager::SendAccessDenied(SessionData rns) const {
        Send(rns, status::bad_request, serializer_->SerializeError("access_denied", "you have no access to do this action"));
    }
    void SendManager::SendNotYourMove(SessionData rns) const {
        Send(rns, status::bad_request, serializer_->SerializeError("not_your_move", "the opponent's move is now"));
    }
    void SendManager::SendWrongMove(SessionData rns) const {
        Send(rns, status::bad_request, serializer_->SerializeError("wrong_move", "player cant make such move"));
    }

    void SendManager::SendWrongUrlParameters(SessionData rns)const {
        Send(rns, status::unprocessable_entity, serializer_->SerializeError("url_parameters_error", "this api function requires url parameters"));
    }

    void SendManager::HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, SessionData rns) const {
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
    
    void SendManager::SendWrongMethod(const ApiFunctionExecutor& executor, SessionData rns)const {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("wrong_method", "method not allowed");
        const std::vector<http::verb>& methods = executor.GetApiFunction().GetAllowedMethods();
        bool can_head = std::find(methods.begin(), methods.end(), http::verb::head) != methods.end();
        rns.session_functions.send_string(builder.BodyText(std::move(body), rns.request.method(), can_head).Status(status::method_not_allowed).Allow(methods).GetProduct());
    }
    void SendManager::SendUndefinedError(const ApiFunctionExecutor& executor, SessionData rns)const {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("undefined_error", "some weird error happened");
        rns.session_functions.send_string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
}