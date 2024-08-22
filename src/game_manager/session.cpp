#include "session.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cmath>

#define BIND(func, place, pls, ...) (Bomb::ExplodeFunc)std::bind( func, this->weak_from_this(), pls) 
namespace game_manager{

    Session::Session(um::Uuid player1, const um::Login& login1, um::Uuid player2, const um::Login& login2) 
        :player1_(std::move(player1)), 
        player2_(std::move(player2)), 
        uuid_to_login_{{player1_, login1},{player2_, login2}},
        state_(std::make_shared<State>()),
        events_wrapper_(std::make_shared<EventListWrapper>()){
        InitSessionState(login1, login2);
    }

    bool Session::HasPlayer(const um::Uuid& uuid){
        return player1_ == uuid || player2_ == uuid;
    }

    State::CPtr Session::GetState(){
        return state_;
    }
    void Session::SetState(State&& state) {
        *state_ = std::move(state);
        events_wrapper_->Clear();
    }
    EventListWrapper::CPtr Session::GetEvents() {
        return events_wrapper_;
    }

    std::optional<Session::ResultsUuid> Session::GetResults() {
        if(scoreboard_.empty())
            return std::nullopt;
        if(uuid_to_login_.at(player1_) == scoreboard_[0]->login)
            return Session::ResultsUuid {
                player1_, player2_
            };
        else
            return Session::ResultsUuid {
                player2_, player1_
            };

    }

    Session::GameApiStatus Session::ApiResign(const um::Uuid& player_id) {
        std::lock_guard<std::mutex> locker(move_mutex_);
        ++state_->move_number;
        AddEvent(player_id == player1_ ? player1().id : player2().id, PLAYER_RESIGN, EmptyData{});
        FinishSession(player_id != player1_);
        AfterMove();
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiWalk(const um::Uuid& player_id, const PlaceData& move_data){
        std::lock_guard<std::mutex> locker(move_mutex_);
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
        ++state_->move_number;
        AddEvent(player.id, PLAYER_WALK, WalkData{std::move(move_data)});
        AfterMove();
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiPlaceBomb(const um::Uuid& player_id, const PlaceData& move_data) {
        std::lock_guard<std::mutex> locker(move_mutex_);
        Player& player = player1().login == uuid_to_login_.at(player_id)?
            player1() : player2();

        if (player.login != nowTurn())
            return GameApiStatus::NotYourMove;

        if (std::abs(short(player.posX - move_data.posX)) > 1 ||
            std::abs(short(player.posY - move_data.posY)) > 1){
            return GameApiStatus::WrongMove;
        } // cell is near the player
        if (!ValidCell(move_data.posX, move_data.posY))
            return GameApiStatus::WrongMove;
        
        Bomb::Ptr bomb = PlaceBombObject(move_data, player.login);
        ++state_->move_number;
        AddEvent(player.id, PLAYER_PLACE_BOMB, BombData{std::move(move_data), {bomb->actor_id}, bomb->ticks_left});
        AfterMove();
        return GameApiStatus::Ok;
    }
    
    void Session::FinishSession(bool firstWinner) {
        if(firstWinner){
            scoreboard_.push_back(&player1());
            scoreboard_.push_back(&player2());
        }
        else{
            scoreboard_.push_back(&player2());
            scoreboard_.push_back(&player1());
        }
    }

    void Session::InitSessionState(const um::Login& login1, const um::Login& login2){
        state_->players.resize(2);
        state_->players = {
            std::make_shared<Player>(),
            std::make_shared<Player>(),
        };
        player1().login = login1;
        player2().login = login2;

        player1().posX = 4;
        player1().posY = 1;

        player2().posX = 3;
        player2().posY = 6;

        player1().id = GetId();
        player2().id = GetId();
        
        std::vector<std::pair<unsigned, unsigned>> walls = {
            {0,2}, {3,2}, 
            {4,3}, {6,3},
            {1,4}, {3,4},
            {4,5}, {7,5}
        };
        for(auto& pair : walls){
            terrain().push_back(std::make_shared<Obstacle>(pair.first, pair.second, Obstacle::Type::Wall));
        }

        nowTurn() = login1;
        state_->map_size = {15,15};    
    }

    void Session::AfterMove(){
        for(auto it = objects().begin(), it_n = it; it != objects().end();){
            if(!scoreboard_.empty()) break;

            std::advance(it_n, 1);

            Object::EventsType events = (*it)->UpdateTick(state_->move_number);
            events_wrapper_->AddEvents(std::move(events));

            it = it_n;
        }
        if(!scoreboard_.empty())
            return AddEvent(scoreboard_[0]->id, PLAYER_WON, EmptyData{});
        nowTurn() = nowTurn() == player1().login? player2().login : player1().login;
    }

    void Session::AddEvent(ActorId actor_id, std::string event_type, VariantEventData&& data) {
        events_wrapper_->AddEvent(Event{state_->move_number, actor_id, std::move(event_type), std::move(data)});
    }

    bool Session::ValidCell(unsigned posX, unsigned posY){
        if (state_->map_size.height <= posY || state_->map_size.width <= posX){
            spdlog::warn("cell is invalid: out of map size [{},{}]. map is {}x{}", posX, posY, state_->map_size.width,state_->map_size.height);
            return false;
        }

        auto it = std::find_if(terrain().begin(), terrain().end(), 
            [&](Obstacle::Ptr v){return v->posX == posX && v->posY == posY;});
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

    Bomb::Ptr Session::PlaceBombObject(PlaceData place, Object::OwnerType login) {
        Bomb::Ptr obj = std::make_shared<Bomb>(login, GetId(), Bomb::Methods{
            [&](ActorId actor_id){
                RemoveObject(actor_id);
            },
            [&](Dimention x, Dimention y){
                Explode(x, y);
            },
        });
        obj->Place(place.posX, place.posY);
        objects().emplace_back(obj);
        return obj;
    }
    Gun::Ptr Session::PlaceGunObject(DirectedPlaceData place, Object::OwnerType login) {
        Gun::Ptr obj = std::make_shared<Gun>(login, GetId(), Gun::Methods{
            [&](ActorId actor_id){
                RemoveObject(actor_id);
            },
            [&](Gun::Ptr gun){
                ShootBullet(gun);
            },
        });
        obj->Place(place.posX, place.posY, place.dir);
        objects().emplace_back(obj);
        return obj;
    }

    Bullet::Ptr Session::PlaceBulletObject(DirectedPlaceData place, Object::OwnerType login) {
        Bullet::Ptr obj = std::make_shared<Bullet>(login, GetId(), Bullet::Methods{
            [&](ActorId actor_id){
                RemoveObject(actor_id);
            },
        });
        obj->Place(place.posX, place.posY, place.dir);
        objects().emplace_back(obj);
        return obj;
    }

    void Session::RemoveObject(ActorId actor_id) {
        objects().erase(std::find_if(objects().begin(), objects().end(), [&](Object::Ptr obj){return obj->actor_id == actor_id;}));
    }

    void Session::Explode(Dimention posX, Dimention posY) {
        for(Dimention x = std::max(0, int(posX) - 1); x <= std::min(state_->map_size.width - 1, posX + 1); ++x){
            for(Dimention y = std::max(0, int(posY) - 1); y <= std::min(state_->map_size.height - 1, posY + 1); ++y){
                if(player1().posX == x && player1().posY == y){
                    return FinishSession(false);
                }
                if(player2().posX == x && player2().posY == y){
                    return FinishSession(true);
                }
            }
        }
    }

    void Session::ShootBullet(Gun::Ptr gun){
        PlaceBulletObject(DirectedPlaceData{gun->posX, gun->posY, gun->dir}, gun->owner);
    }

    std::optional<IShootable::Arr> Session::GetShootableObjects(Bullet::Ptr bullet) {
        if(bullet->posX < 0 || bullet->posY < 0 || 
            bullet->posX >= state_->map_size.width || 
            bullet->posY >= state_->map_size.height)
            return std::nullopt;

        IShootable::Arr res;

        std::copy(terrain().begin(), terrain().end(), std::back_inserter(res));
        std::copy(objects().begin(), objects().end(), std::back_inserter(res));

        return res;
    }
}