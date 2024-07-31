#pragma once
#include <string>
#include <map>
#include "user.hpp"
#include "state.hpp"

namespace game_manager{
    using SessionId = std::string;
    class Session{
    public:
        Session(um::Uuid player1, const um::Login& login1, um::Uuid player2, const um::Login& login2);

        bool HasPlayer(const um::Uuid& uuid);

        State::CPtr GetState();

        struct Results{
            um::Uuid winner;
            um::Uuid loser;
        };

        Results GetResults();

        enum MoveType{
            Walk,
            Resign
        };
        
        enum GameApiStatus{
            Ok,
            WrongMove,
            NotYourMove,
            Finish
        };

        struct MoveData{
            unsigned posX;
            unsigned posY;
        };

        struct WalkData : public MoveData{};
        GameApiStatus ApiWalk(const um::Uuid& player_id, const WalkData& move_data);

        GameApiStatus ApiResign(const um::Uuid& player_id);

    private:
        void FinishSession(bool firstWinner);

        void InitSessionState(const um::Login& login1, const um::Login& login2);
        void AfterMove();
        bool ValidCell(unsigned posX, unsigned posY);

        State::Ptr state_;

        um::Uuid player1_;
        um::Uuid player2_;

        Results results_;

        Player& player1(){return state_->players[0];}
        Player& player2(){return state_->players[1];}
        um::Login& nowTurn(){return state_->now_turn;}
        State::Terrain& terrain(){return state_->terrain;}

        const std::map<um::Uuid, um::Login> uuid_to_login_;
    };
}   
namespace gm = game_manager;