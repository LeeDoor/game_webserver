#pragma once
#include <string>
#include <map>
#include "user_data.hpp"
#include "state.hpp"

namespace game_manager{
    using SessionId = std::string;
    class Session{
    public:
        Session(dm::Uuid player1, const dm::Login& login1, dm::Uuid player2, const dm::Login& login2);

        bool HasPlayer(const dm::Uuid& uuid);

        State::CPtr GetState();

        enum PlayerMoveType{
            Walk
        };
        
        enum GameApiStatus{
            Ok,
            WrongMove,
            NotYourMove
        };

        struct GameApiData{
            unsigned posX;
            unsigned posY;
        };

        struct WalkData : public GameApiData{};
        GameApiStatus ApiWalk(const dm::Uuid& player_id, const WalkData& move_data);

    private:
        void AfterMove();

        State::Ptr state_;

        dm::Uuid player1_;
        dm::Uuid player2_;

        Player& StatePlayer1(){return state_->players[0];}
        Player& StatePlayer2(){return state_->players[1];}

        const std::map<dm::Uuid, dm::Login> uuid_to_login_;
    };
}   
namespace gm = game_manager;