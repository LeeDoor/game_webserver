#include "socket_functions.hpp"
#include "api_functions.hpp"

#include "session.hpp"


TEST_CASE("ApiSessionState", "[api][game][session_state]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    
    std::string WRONG_SESSIONID = serializer::SerializeError("wrong_sessionId", "no session with such sessionId");
    std::string URL_PARAMETERS_ERROR = serializer::SerializeError("url_parameters_error", "this api function requires url parameters");
    std::string UNAUTHORIZED = serializer::SerializeError("unathorized", "request must be authorized");

    LoginData ld2;
    if(MMQueueSuccess(socket).size() == 0)
        ld2 = EnqueueNewPlayer(socket);
    else{
        auto ud = UserSuccess(socket, MMQueueSuccess(socket)[0]);
        ld2 = LoginSuccess(socket, ud.login);
    }
    LoginData ld = EnqueueNewPlayer(socket);
    gm::SessionId sid = WaitForOpponentSuccess(socket, ld.token);

    SECTION ("success"){
        gm::State state = SessionStateSuccess(socket, ld.token, sid);
        REQUIRE(state == SessionStateSuccess(socket, ld2.token, sid));

        REQUIRE(state.players.size() == 2);
        bool now_turn_match = state.now_turn == state.players[0].login || state.now_turn == state.players[1].login;
        REQUIRE(now_turn_match);

        hh::RegistrationData rd = RegisterSuccess(socket);
        LoginData ld1 = LoginSuccess(socket, rd.login);
        gm::State state2 = SessionStateSuccess(socket, ld1.token, sid);
        REQUIRE(state == state2);
    }
    SECTION ("success_multiple_sessions"){
        std::vector<LoginData> users;
        std::vector<gm::SessionId> sessions;
        std::vector<gm::State> states;
        users.resize(10);
        sessions.resize(10);
        states.resize(10);
        for(LoginData& ld : users)
            ld = EnqueueNewPlayer(socket);
        for(int i = 0; i < 10; ++i)
            sessions[i] = WaitForOpponentSuccess(socket, users[i].token);
        REQUIRE(sessions[0] == sessions[1]); // same session has same sessionId
        for(int i = 1; i < 10; i+=2){
            REQUIRE(sessions[i] != sessions[i + 1]); // each session is unique
            REQUIRE(sessions[i] == sessions[i - 1]); // same session has same sessionId
        }

        for(int i = 0; i < 10; ++i){
            states[i] = SessionStateSuccess(socket, users[i].token, sessions[i]);
        }
        for(int i = 0; i < 10; i+=2){
            REQUIRE(states[i] == states[i + 1]);
            gm::State& state = states[i];
            um::Login& nt = state.now_turn;
            gm::State::Players& players = state.players;
            REQUIRE(players.size() == 2);
            gm::Player& player1 = players[0];
            gm::Player& player2 = players[1];
            REQUIRE(player1 != player2);
            // now_turn fits first player
            bool now_turn_fit = nt == player1.login;
            //now_turn fits second player
            now_turn_fit = now_turn_fit || nt == player2.login;
            REQUIRE(now_turn_fit);
        }
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