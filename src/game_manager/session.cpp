#include "session.hpp"
#include <cmath>

namespace game_manager{

    Session::Session(dm::Uuid player1, const dm::Login& login1, dm::Uuid player2, const dm::Login& login2) 
        :player1_(std::move(player1)), 
        player2_(std::move(player2)), 
        uuid_to_login_{{player1_, login1},{player2_, login2}},
        state_(std::make_shared<State>()){

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

    Session::GameApiStatus Session::ApiWalk(const dm::Uuid& player_id, const WalkData& move_data){
        Player& player = StatePlayer1().login == uuid_to_login_.at(player_id)?
            StatePlayer1() : StatePlayer2();
        if (player.login != state_->now_turn)
            return GameApiStatus::NotYourMove;
        if (player.posX == move_data.posX && std::abs(static_cast<short>(player.posY - move_data.posY)) == 1 ||
            player.posY == move_data.posY && std::abs(static_cast<short>(player.posX - move_data.posX)) == 1){
            // sum of changes #TODO
            player.posX = move_data.posX;
            player.posY = move_data.posY;
            AfterMove();
            return GameApiStatus::Ok;
        }
        return GameApiStatus::WrongMove;
    }
    
    void Session::AfterMove(){
        state_->now_turn = state_->now_turn == StatePlayer1().login? StatePlayer2().login : StatePlayer1().login;
    }

}