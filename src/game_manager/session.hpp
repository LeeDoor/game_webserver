#pragma once
#include <string>
#include <map>
#include "user_data.hpp"
#include "state.hpp"

namespace game_manager{
    using SessionId = std::string;
    class Session{
    public:
        Session(dm::Uuid&& player1, const dm::Login& login1, dm::Uuid&& player2, const dm::Login& login2);

        bool HasPlayer(const dm::Uuid& uuid);

        State::CPtr GetState();
        
        enum GameApiStatus{
            Ok,
            WrongMove,
            NotYourMove
        };

        struct GameApiData{
            unsigned posX;
            unsigned posY;
        };

        struct MoveData : public GameApiData{};
        GameApiStatus Move(const dm::Uuid& player_id, const MoveData& move_data);

    private:
        State::Ptr state_;

        dm::Uuid player1_;
        dm::Uuid player2_;

        const std::map<dm::Uuid, dm::Login> uuid_to_login_;
    };
}   
namespace gm = game_manager;