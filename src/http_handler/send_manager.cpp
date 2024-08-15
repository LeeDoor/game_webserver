#include "send_manager.hpp"
#include "send_manager_gm.hpp"
#include "send_manager_sm.hpp"
#include "send_manager_tm.hpp"
#include "send_manager_um.hpp"
#include "send_manager_http.hpp"
#include "spdlog/spdlog.h"
#include "serializer_basic.hpp"
#include "serializer_user.hpp"
#include "serializer_game.hpp"
#include "serializer_http.hpp"
#include "serializer_session.hpp"
#include "response_builder.hpp"

namespace http_handler{
    void Send(SessionData rns, status stat, std::string body) {
        ResponseBuilder<http::string_body> builder;
        spdlog::info("SENT [{}] to {}: {}", static_cast<int>(stat), rns.request.target().to_string(), body);  
        StringResponse response = builder.BodyText(std::move(body), rns.request.method()).Status(stat).GetProduct();
        rns.session_functions.send_string(std::move(response));
    }
    
    void SendSuccess(SessionData rns) {
        Send(rns, status::ok, serializer::SerializeEmpty());
    }
    void SendToken(SessionData rns, token_manager::Token& token) {
        Send(rns, status::ok, serializer::SerializeMap({{"token", token}}));
    }
    void SendUser(SessionData rns, const PublicUser& puser) {
        Send(rns, status::ok, serializer::Serialize(puser));
    }
    void SendHiddenUser(SessionData rns, const um::User& user) {
        Send(rns, status::ok, serializer::Serialize(user));
    }
    void SendSessionId(SessionData rns, const gm::SessionId& session_id) {
        Send(rns, status::ok, serializer::SerializeMap({{"sessionId", session_id}}));
    }
    void SendGameState(SessionData rns, const gm::State& state) {
        Send(rns, status::ok, serializer::Serialize(state));
    }
    void SendFinishedState(SessionData rns, const session_manager::PublicSessionData& data) {
        Send(rns, status::ok, serializer::Serialize(data));
    }
    void SendEvents(SessionData rns, const gm::EventListWrapper& events, int from_move) {
        Send(rns, status::ok, serializer::Serialize(events.FromMove(from_move)));
    }

    void SendWrongApiFunction(SessionData rns) {
        Send(rns, status::bad_request, serializer::SerializeError("api_error", "wrong api function"));
    }
    void SendWrongBodyData(SessionData rns) {
        Send(rns, status::bad_request, serializer::SerializeError("body_data_error", "wrong body data"));
    }
    void SendLoginTaken(SessionData rns) {
        Send(rns, status::conflict, serializer::SerializeError("login_taken", "login is already taken"));
    }
    void SendWrongLoginOrPassword(SessionData rns) {
        Send(rns, status::bad_request, serializer::SerializeError("wrong_login_or_password", "login size >= 3 password size >= 6 with digit(s)"));
    }
    void SendNoSuchUser(SessionData rns) {
        Send(rns, status::bad_request, serializer::SerializeError("no_such_user", "no user with this login or password"));
    }
    void SendUnauthorized(SessionData rns) {
        Send(rns, status::unauthorized, serializer::SerializeError("unathorized", "request must be authorized"));
    }
    void SendInvalidToken(SessionData rns) {
        Send(rns, status::unauthorized, serializer::SerializeError("invalid_token", "request authorization is invalid"));
    }
    void SendAdminUnrecognized(SessionData rns) {
        Send(rns, status::unauthorized, serializer::SerializeError("invalid_admin", "the administrator password is missing or incorrect"));
    }
    void SendTokenToRemovedPerson(SessionData rns) {
        Send(rns, status::unauthorized, serializer::SerializeError("person_removed", "person with this token is unavailable (prob. removed)"));
    }
    void SendCantEnqueue(SessionData rns) {
        Send(rns, status::ok, serializer::SerializeError("enqueue_error", "error happened while enqueuing player (already in queue)"));
    }
    void SendInTheMatch(SessionData rns, const gm::SessionId& sid) {
        Send(rns, status::bad_request, serializer::SerializeMap({
            {"error_name", "in_the_match"},
            {"sessionId", sid}
        }));
    }
    void SendCantLogin(SessionData rns) {
        Send(rns, status::service_unavailable, serializer::SerializeError("login_error", "unable to add token to database."));
    }
    void SendPollClosed(SessionData rns, const std::string& description) {
        Send(rns, status::conflict, serializer::SerializeError("poll_closed", description));
    }
    void SendWrongSessionId(SessionData rns) {
        Send(rns, status::bad_request, serializer::SerializeError("wrong_sessionId", "no session with such sessionId"));
    }
    void SendAccessDenied(SessionData rns) {
        Send(rns, status::bad_request, serializer::SerializeError("access_denied", "you have no access to do this action"));
    }
    void SendNotYourMove(SessionData rns) {
        Send(rns, status::bad_request, serializer::SerializeError("not_your_move", "the opponent's move is now"));
    }
    void SendWrongMove(SessionData rns) {
        Send(rns, status::bad_request, serializer::SerializeError("wrong_move", "player cant make such move"));
    }
    void SendSessionFinished(SessionData rns) {
        Send(rns, status::bad_request, serializer::SerializeError("session_finished", "session is finished"));
    }

    void SendWrongUrlParameters(SessionData rns) {
        Send(rns, status::unprocessable_entity, serializer::SerializeError("url_parameters_error", "this api function requires url parameters"));
    }


    void HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, SessionData rns) {
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
    
    void SendWrongMethod(const ApiFunctionExecutor& executor, SessionData rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer::SerializeError("wrong_method", "method not allowed");
        const std::vector<http::verb>& methods = executor.GetApiFunction().GetAllowedMethods();
        bool can_head = std::find(methods.begin(), methods.end(), http::verb::head) != methods.end();
        rns.session_functions.send_string(builder.BodyText(std::move(body), rns.request.method(), can_head).Status(status::method_not_allowed).Allow(methods).GetProduct());
    }
    void SendUndefinedError(const ApiFunctionExecutor& executor, SessionData rns) {
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer::SerializeError("undefined_error", "some weird error happened");
        rns.session_functions.send_string(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct());
    }
}