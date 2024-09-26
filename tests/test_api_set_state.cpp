#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "nlohmann/json.hpp"
#include "type_serializer.hpp"

TEST_CASE("ApiSetState", "[api][debug][set_state]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    
    std::string UNAUTHORIZED = serializer::SerializeError("invalid_admin", "the administrator password is missing or incorrect");
    std::string ENQUEUE_ERROR = serializer::SerializeError("enqueue_error", "error happened while enqueuing player (already in queue)");

    SECTION("success"){
        SessionData sd = CreateNewMatch(socket);
        gm::State& new_state = sd.state;
        new_state.map_size = {12, 12};
        new_state.move_number = 228;
        new_state.terrain = {std::make_shared<gm::Obstacle>(gm::Obstacle{{1,1},gm::Obstacle::Type::Wall}), std::make_shared<gm::Obstacle>(gm::Obstacle{{2,2},gm::Obstacle::Type::Wall})};
        new_state.players.front()->position.x += 2;
        new_state.players.front()->position.y += 2;
        new_state.players.back()->position.x += 2;
        new_state.players.back()->position.y += 2;
        SetStateSuccess(socket, new_state, sd.sid);
        gm::State changed_state = SessionStateSuccess(socket, sd.l1.token, sd.sid);
        
        nlohmann::json jbefore(new_state), jafter(changed_state);
        INFO(jbefore.dump());
        INFO(jafter.dump());
        CHECK(changed_state == new_state);
    }
}