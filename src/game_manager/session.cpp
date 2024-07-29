#include "session.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
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
        state_->map_size = {15,15};
    }

    bool Session::HasPlayer(const dm::Uuid& uuid){
        return player1_ == uuid || player2_ == uuid;
    }

    State::CPtr Session::GetState(){
        return state_;
    }

    Session::GameApiStatus Session::ApiWalk(const dm::Uuid& player_id, const WalkData& move_data){
        Player& player = player1().login == uuid_to_login_.at(player_id)?
            player1() : player2();

        if (player.login != nowTurn())
            return GameApiStatus::NotYourMove;

        if (std::abs(short(player.posX - move_data.posX))
          + std::abs(short(player.posY - move_data.posY)) != 1){
            return GameApiStatus::WrongMove;
        } // cell is near the player
        if (!ValidCell(move_data.posX, move_data.posY))
            return GameApiStatus::WrongMove;
        player.posX = move_data.posX;
        player.posY = move_data.posY;
        AfterMove();
        return GameApiStatus::Ok;
    }
    
    void Session::AfterMove(){
        nowTurn() = nowTurn() == player1().login? player2().login : player1().login;
    }

    bool Session::ValidCell(unsigned posX, unsigned posY){
        if (state_->map_size.height <= posY || state_->map_size.width <= posX){
            spdlog::warn("cell is invalid: out of map size [{},{}]. map is {}x{}", posX, posY, state_->map_size.width,state_->map_size.height);
            return false;
        }

        auto it = std::find_if(terrain().begin(), terrain().end(), 
            [&](const Obstacle& v){return v.posX == posX && v.posY == posY;});
        if(it != terrain().end()){
            spdlog::warn("cell is invalid: on the obstacle [{},{}]", posX, posY);
            return false;
        }
        
        if(player1().posX == posX && player1().posY == posY ||
           player2().posX == posX && player2().posY == posY){
            spdlog::warn("cell is invalid: on the other player [{},{}]", posX, posY);
            return false;
        }

        return true;
    }
}