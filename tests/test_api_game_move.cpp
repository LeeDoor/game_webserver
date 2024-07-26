#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "json_serializer.hpp"
#include "session.hpp"
using namespace serializer;

bool ValidCell(const gm::State& state, unsigned x, unsigned y){
    auto& terrain = state.terrain;
    
    auto it = std::find_if(terrain.begin(), terrain.end(), 
        [&](const gm::Obstacle& o){return o.posX == x && o.posY == y;});
    return it == terrain.end();
}

TEST_CASE("ApiMove", "[api][game][move]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string WRONG_MOVE = serializer->SerializeError("wrong_move", "player cant make such move");
    std::string NOT_YOUR_MOVE = serializer->SerializeError("not_your_move", "the opponent's move is now");
    std::string ACCESS_DENIED = serializer->SerializeError("access_denied", "you have no access to do this action");
    std::string NO_SUCH_SESSION = serializer->SerializeError("no_such_session", "session you are trying to get access to does not exist");
    std::string WRONG_BODY_DATA = serializer->SerializeError("body_data_error", "wrong body data");

    SECTION("success"){
        if(MMQueueSuccess(socket, serializer).size() == 1)
            EnqueueNewPlayer(socket, serializer);
        LoginData ld1 = EnqueueNewPlayer(socket, serializer);
        LoginData ld2 = EnqueueNewPlayer(socket, serializer);
        gm::SessionId sid = WaitForOpponentSuccess(socket, ld1.token, serializer);
        REQUIRE(sid == WaitForOpponentSuccess(socket, ld2.token, serializer));
        std::optional<dm::Login> prev_turn = std::nullopt;
        for(int i = 0; i < 10; ++i){
            gm::State state = SessionStateSuccess(socket, serializer, ld1.token, sid);
            REQUIRE(state == SessionStateSuccess(socket, serializer, ld2.token, sid));

            dm::Login& now_turn = state.now_turn;
            LoginData& ld = ld1.login == now_turn ? ld1 : ld2;
            gm::Player& player = state.players[0].login == now_turn ? state.players[0] : state.players[1];

            if(prev_turn)
                CHECK(*prev_turn != now_turn);

            std::vector<gm::Session::WalkData> wds{
                {player.posX + 1, player.posY},
                {player.posX, player.posY + 1},
                {player.posX - 1, player.posY},
                {player.posX, player.posY - 1},
            }; 
            for(auto& wd : wds){
                if (ValidCell(state, wd.posX, wd.posY)){
                    WalkSuccess(socket, serializer, wd, ld.token, sid);
                    break;
                }
            }
            prev_turn = std::string(now_turn);
        }
    }

}