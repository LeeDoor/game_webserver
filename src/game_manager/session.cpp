#include "session.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cmath>
#include "custom_events.hpp"
#include "session_api_director.hpp"
#include "state.hpp"

namespace game_manager{
    std::map<MoveType, SessionApiValidator> Session::api_validator_ = {};

    Session::Session(um::Uuid player1, const um::Login& login1, um::Uuid player2, const um::Login& login2) 
        :player1_(std::move(player1)), 
        player2_(std::move(player2)), 
        uuid_to_login_{{player1_, login1},{player2_, login2}}{
        state_->Init(login1, login2);
        move_api_ = std::make_shared<State>(events_wrapper_);
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
        return move_api_->GetState();
    }
    void Session::SetState(State&& state) {
        state.UpdateStatePointers();
        events_wrapper_->Clear();
        state.events_wrapper_ = events_wrapper_;
        state.id_counter_ = 2;
        move_api_->SetState(std::move(state));
    }
    EventListWrapper::CPtr Session::GetEvents() {
        return events_wrapper_;
    }

    std::optional<Session::ResultsUuid> Session::GetResults() {
        /*if(state_->scoreboard_.empty())
            return std::nullopt;
        if(uuid_to_login_.at(player1_) == state_->scoreboard_[0].lock()->login)
            return Session::ResultsUuid {
                player1_, player2_
            };
        else
            return Session::ResultsUuid {
                player2_, player1_
            };*/

    }

    GameApiStatus Session::ApiMove(const um::Uuid& player_id, MoveData md) {
        std::lock_guard<std::mutex> locker(move_mutex_);
        Player::Ptr player =  player1()->login == uuid_to_login_.at(player_id)? player1() : player2();
        if(GameApiStatus status = api_validator_.at(md.move_type)(move_api_->GetState(), player, md); status != GameApiStatus::Ok)
            return status; // fix
        //state_->IncreaseMoveNumber();
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
        AfterMove();
        return GameApiStatus::Ok;
    }

    void Session::ApiResign(Player::Ptr player, MoveData md) {
        move_api_->FinishSession(player1()->actor_id != player->actor_id);
        events_wrapper_->AddEvent(EmptyEvent({player->actor_id, PLAYER_RESIGN}));
    }
    void Session::ApiWalk(Player::Ptr player, MoveData md){
        player->position.x = md.position.x;
        player->position.y = md.position.y;
        events_wrapper_->AddEvent(WalkEvent({{player->actor_id, PLAYER_WALK}, player->position}));
    }
    void Session::ApiPlaceBomb(Player::Ptr player, MoveData md) {
        Bomb::Ptr bomb = move_api_->PlaceBombObject(md.position, player->login);
        events_wrapper_->AddEvent(BombEvent({{{player->actor_id, PLAYER_PLACE_BOMB}, bomb->position}, bomb->actor_id}));
    }
    void Session::ApiPlaceGun(Player::Ptr player, MoveData md) {
        Gun::Ptr gun = move_api_->PlaceGunObject(md.position, md.direction, player->login);
        events_wrapper_->AddEvent(GunEvent({{{{player->actor_id, PLAYER_PLACE_GUN}, gun->position}, gun->actor_id}, gun->direction}));
    }

    void Session::AfterMove() {
        move_api_->AfterMove();
    }
}