#include "session.hpp"

namespace game_manager{

    Session::Session(dm::Uuid&& player1, const dm::Login& login1, dm::Uuid&& player2, const dm::Login& login2) 
        :player1_(std::move(player1)), player2_(std::move(player2)), uuid_to_login_{{player1, login1},{player2, login2}}{

        state_ = std::make_shared<State>();

        state_->players.resize(2);
        state_->players[0].login = login1;
        state_->players[0].posX = 5;
        state_->players[1].login = login2;
        state_->terrain.resize(2);
        state_->terrain[0].posX = 2;
        state_->terrain[0].posY = 3;
        state_->terrain[0].type = Obstacle::Type::Wall;
        state_->terrain[1].posX = 4;
        state_->terrain[1].posY = 5;
        state_->terrain[1].type = Obstacle::Type::Wall;
        state_->now_turn = login1;
    }

    bool Session::HasPlayer(const dm::Uuid& uuid){
        return player1_ == uuid || player2_ == uuid;
    }

    State::CPtr Session::GetState(){
        return state_;
    }

    Session::GameApiStatus Session::Move(const dm::Uuid& player_id, const MoveData& move_data){
        return GameApiStatus::Ok;
    }
}