#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "json_serializer.hpp"
#include "session.hpp"
using namespace serializer;

TEST_CASE("ApiSessionState", "[api][game][session_state]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string WRONG_SESSIONID = serializer->SerializeError("wrong_sessionId", "no session with such sessionId");
    std::string URL_PARAMETERS_ERROR = serializer->SerializeError("url_parameters_error", "this api function requires url parameters");
    std::string UNAUTHORIZED = serializer->SerializeError("unathorized", "request must be authorized");

    LoginData ld2;
    if(MMQueueSuccess(socket, serializer).size() == 0)
        ld2 = EnqueueWorthyOpponent(socket, serializer);
    else{
        auto ud = UserDataSuccess(socket, serializer, MMQueueSuccess(socket, serializer)[0]);
        ld2 = LoginSuccess(socket, ud.login, serializer);
    }
    LoginData ld = EnqueueWorthyOpponent(socket, serializer);
    gm::SessionId sid = WaitForOpponentSuccess(socket, ld.token, serializer);

    SECTION ("success"){
        gm::State state = SessionStateSuccess(socket, serializer, ld.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, serializer, ld2.token, sid));

        REQUIRE(state.players.size() == 2);
        bool now_turn_match = state.now_turn == state.players[0].login || state.now_turn == state.players[1].login;
        REQUIRE(now_turn_match);

        hh::RegistrationData rd = RegisterSuccess(socket, serializer);
        LoginData ld1 = LoginSuccess(socket, rd.login, serializer);
        gm::State state2 = SessionStateSuccess(socket, serializer, ld1.token, sid);
        REQUIRE(state == state2);
    }
    SECTION ("wrong_sessionId"){
        std::string target = SetUrlParameters(SESSION_STATE_API, {{"sessionId", "ABOBUS227"}});
        http::request<http::string_body> request{http::verb::get, target, 11};
        SetAuthorizationHeader(request, ld.token);
        auto response = GetResponseToRequest(false, request, socket);
        CheckStringResponse(response, {
            .body=WRONG_SESSIONID,
            .res=http::status::bad_request,
        });
    }
    SECTION("url_parameters_error") {
        http::request<http::string_body> request = {http::verb::get, SESSION_STATE_API, 11};

        SetAuthorizationHeader(request, ld.token);
        auto response = GetResponseToRequest(false, request, socket);
        CheckStringResponse(response, {
            .body=URL_PARAMETERS_ERROR,
            .res=http::status::unprocessable_entity,
        });
    }
    SECTION ("not_logined"){
        std::string target = SetUrlParameters(SESSION_STATE_API, {{"sessionId", sid}});
        http::request<http::string_body> request{http::verb::get, target, 11};

        auto response = GetResponseToRequest(false, request, socket);
        CheckStringResponse(response, {
            .body=UNAUTHORIZED,
            .res=http::status::unauthorized,
        });
    }

}