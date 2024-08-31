#include "session.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cmath>
#include "custom_events.hpp"
#include "session_api_director.hpp"

namespace game_manager{
    std::map<MoveType, SessionApiValidator> Session::api_validator_ = {};

    Session::Session(um::Uuid player1, const um::Login& login1, um::Uuid player2, const um::Login& login2) 
        :player1_(std::move(player1)), 
        player2_(std::move(player2)), 
        uuid_to_login_{{player1_, login1},{player2_, login2}},
        state_(std::make_shared<State>()){
        state_->Init(login1, login2);
    }

    void Session::InitApi() {
        SessionApiDirector director;
        api_validator_ = {
            {MoveType::Walk, director.BuildWalk()},
            {MoveType::Resign, director.BuildResign()},
            {MoveType::PlaceBomb, director.BuildPlaceBomb()},
            {MoveType::PlaceGun, director.BuildPlaceGun()}
        };
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
        std::lock_guard<std::mutex> locker(move_mutex_);
        Player::Ptr player =  player1()->login == uuid_to_login_.at(player_id)? player1() : player2();
        if(!api_validator_.at(md.move_type)(state_, player, md))
            return GameApiStatus::WrongMove; // fix
        switch(md.move_type){
        case MoveType::Walk:
            ApiWalk(player, md);
            break;
        case MoveType::Resign:
            ApiResign(player, md);
            break;
        case MoveType::PlaceBomb:
            ApiPlaceBomb(player, md);
            break;
        case MoveType::PlaceGun:
            ApiPlaceGun(player, md);
            break;
        }
        ++state_->move_number;
        switch(md.move_type){
        case MoveType::Walk:
            state_->events_wrapper_->AddEvent(WalkEvent({{state_->move_number, player->actor_id, PLAYER_WALK}, player->position}));
            break;
        case MoveType::Resign:
            state_->events_wrapper_->AddEvent(EmptyEvent({state_->move_number, player_id == player1_ ? player1()->actor_id : player2()->actor_id, PLAYER_RESIGN}));
            break;
        case MoveType::PlaceBomb:
            state_->events_wrapper_->AddEvent(BombEvent({{{state_->move_number, player->actor_id, PLAYER_PLACE_BOMB}, bomb->position}, bomb->actor_id}));
            break;
        case MoveType::PlaceGun:
            state_->events_wrapper_->AddEvent(GunEvent({{{{state_->move_number, player->actor_id, PLAYER_PLACE_GUN}, md.position}, gun->actor_id}, md.direction}));
            break;
        }
        state_->AfterMove();
    }

    Session::GameApiStatus Session::ApiResign(Player::Ptr player, MoveData md) {
        state_->FinishSession(player_id != player1_);
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiWalk(Player::Ptr player, MoveData md){
        player->position.x = md.position.x;
        player->position.y = md.position.y;
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiPlaceBomb(Player::Ptr player, MoveData md) {
        Bomb::Ptr bomb = state_->PlaceBombObject(md.position, player->login);
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiPlaceGun(Player::Ptr player, MoveData md) {
        Gun::Ptr gun = state_->PlaceGunObject(md.position, md.direction, player->login);
        return GameApiStatus::Ok;
    }
}