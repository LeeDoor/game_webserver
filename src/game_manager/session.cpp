#include "session.hpp"

namespace game_manager{

    Session::Session(dm::Uuid&& player1, const dm::Login& login1, dm::Uuid&& player2, const dm::Login& login2) 
        :player1_(player1), player2_(player2){

        state_ = std::make_shared<State>();

        state_->players.resize(2);
        state_->players[0].login = login1;
        state_->players[1].login = login2;
        state_->now_turn = login1;
    }

    bool Session::HasPlayer(const dm::Uuid& uuid){
        return player1_ == uuid || player2_ == uuid;
    }

    State::CPtr Session::GetState(){
        return state_;
    }

    bool Session::Ping(const dm::Uuid& player_id){
        return true;
    }
}