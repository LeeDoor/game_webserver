#include "session.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cmath>
#include "custom_events.hpp"

#define BIND(func, place, pls, ...) (Bomb::ExplodeFunc)std::bind( func, this->weak_from_this(), pls) 
namespace game_manager{
    /*const std::map<MoveType, SessionApiExecutor> Session::session_api_ = {
        
    };*/

    Session::Session(um::Uuid player1, const um::Login& login1, um::Uuid player2, const um::Login& login2) 
        :player1_(std::move(player1)), 
        player2_(std::move(player2)), 
        uuid_to_login_{{player1_, login1},{player2_, login2}},
        state_(std::make_shared<State>()){
        state_->Init(login1, login2);
    }

    bool Session::HasPlayer(const um::Uuid& uuid){
        return player1_ == uuid || player2_ == uuid;
    }

    State::CPtr Session::GetState(){
        return state_;
    }
    void Session::SetState(State&& state) {
        *state_ = std::move(state);
        state_->UpdateStatePointers();
        state_->events_wrapper_->Clear();
    }
    EventListWrapper::CPtr Session::GetEvents() {
        return state_->events_wrapper_;
    }

    std::optional<Session::ResultsUuid> Session::GetResults() {
        if(state_->scoreboard_.empty())
            return std::nullopt;
        if(uuid_to_login_.at(player1_) == state_->scoreboard_[0].lock()->login)
            return Session::ResultsUuid {
                player1_, player2_
            };
        else
            return Session::ResultsUuid {
                player2_, player1_
            };

    }

    Session::GameApiStatus Session::ApiMove(const um::Uuid& player_id, MoveData md) {
        switch(md.move_type){
        case MoveType::Walk:
            return ApiWalk(player_id, md);
        case MoveType::Resign:
            return ApiResign(player_id, md);
        case MoveType::PlaceBomb:
            return ApiPlaceBomb(player_id, md);
        case MoveType::PlaceGun:
            return ApiPlaceGun(player_id, md);
        }
        throw std::runtime_error("MoveType not implemented in Session::ApiMove");
    }

    Session::GameApiStatus Session::ApiResign(const um::Uuid& player_id, MoveData md) {
        std::lock_guard<std::mutex> locker(move_mutex_);
        ++state_->move_number;
        state_->events_wrapper_->AddEvent(EmptyEvent({state_->move_number, player_id == player1_ ? player1()->actor_id : player2()->actor_id, PLAYER_RESIGN}));
        state_->FinishSession(player_id != player1_);
        state_->AfterMove();
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiWalk(const um::Uuid& player_id, MoveData md){
        std::lock_guard<std::mutex> locker(move_mutex_);
        Player& player = player1()->login == uuid_to_login_.at(player_id)?
            *player1() : *player2();

        if (player.login != state_->now_turn)
            return GameApiStatus::NotYourMove;

        if (std::abs(short(player.position.x - md.position.x))
          + std::abs(short(player.position.y - md.position.y)) != 1){
            return GameApiStatus::WrongMove;
        } // cell is near the player
        if (!state_->ValidCell(md.position))
            return GameApiStatus::WrongMove;
        
        player.position.x = md.position.x;
        player.position.y = md.position.y;
        ++state_->move_number;
        state_->events_wrapper_->AddEvent(WalkEvent({{state_->move_number, player.actor_id, PLAYER_WALK}, player.position}));
        state_->AfterMove();
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiPlaceBomb(const um::Uuid& player_id, MoveData md) {
        std::lock_guard<std::mutex> locker(move_mutex_);
        Player& player = player1()->login == uuid_to_login_.at(player_id)?
            *player1() : *player2();

        if (player.login != state_->now_turn)
            return GameApiStatus::NotYourMove;

        if (std::abs(short(player.position.x - md.position.x)) > 1 ||
            std::abs(short(player.position.y - md.position.y)) > 1){
            return GameApiStatus::WrongMove;
        } // cell is near the player
        if (!state_->ValidCell(md.position))
            return GameApiStatus::WrongMove;
        
        Bomb::Ptr bomb = state_->PlaceBombObject(md.position, player.login);
        ++state_->move_number;
        state_->events_wrapper_->AddEvent(BombEvent({{{state_->move_number, player.actor_id, PLAYER_PLACE_BOMB}, bomb->position}, bomb->actor_id}));
        state_->AfterMove();
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiPlaceGun(const um::Uuid& player_id, MoveData md) {
        std::lock_guard<std::mutex> locker(move_mutex_);
        Player& player = player1()->login == uuid_to_login_.at(player_id)?
            *player1() : *player2();

        if (player.login != state_->now_turn)
            return GameApiStatus::NotYourMove;

        if (std::abs(short(player.position.x - md.position.x)) > 1 ||
            std::abs(short(player.position.y - md.position.y)) > 1){
            return GameApiStatus::WrongMove;
        } // cell is near the player
        if (!state_->ValidCell(md.position))
            return GameApiStatus::WrongMove;
        
        Gun::Ptr gun = state_->PlaceGunObject(md.position, md.direction, player.login);
        ++state_->move_number;
        state_->events_wrapper_->AddEvent(GunEvent({{{{state_->move_number, player.actor_id, PLAYER_PLACE_GUN}, md.position}, gun->actor_id}, md.direction}));
        state_->AfterMove();
        return GameApiStatus::Ok;
    }
}