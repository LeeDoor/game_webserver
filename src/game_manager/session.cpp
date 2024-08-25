#include "session.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cmath>
#include "custom_events.hpp"

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
        events_wrapper_->AddEvent(EmptyEvent({state_->move_number, player_id == player1_ ? player1().actor_id : player2().actor_id, PLAYER_RESIGN}));
        FinishSession(player_id != player1_);
        AfterMove();
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiWalk(const um::Uuid& player_id, PosMoveData move_data){
        std::lock_guard<std::mutex> locker(move_mutex_);
        Player& player = player1().login == uuid_to_login_.at(player_id)?
            player1() : player2();

        if (player.login != nowTurn())
            return GameApiStatus::NotYourMove;

        if (std::abs(short(player.position.x - move_data.position.x))
          + std::abs(short(player.position.y - move_data.position.y)) != 1){
            return GameApiStatus::WrongMove;
        } // cell is near the player
        if (!ValidCell(move_data.position))
            return GameApiStatus::WrongMove;
        
        player.position.x = move_data.position.x;
        player.position.y = move_data.position.y;
        ++state_->move_number;
        events_wrapper_->AddEvent(WalkEvent({{state_->move_number, player.actor_id, PLAYER_WALK}, player.position}));
        AfterMove();
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiPlaceBomb(const um::Uuid& player_id, PosMoveData move_data) {
        std::lock_guard<std::mutex> locker(move_mutex_);
        Player& player = player1().login == uuid_to_login_.at(player_id)?
            player1() : player2();

        if (player.login != nowTurn())
            return GameApiStatus::NotYourMove;

        if (std::abs(short(player.position.x - move_data.position.x)) > 1 ||
            std::abs(short(player.position.y - move_data.position.y)) > 1){
            return GameApiStatus::WrongMove;
        } // cell is near the player
        if (!ValidCell(move_data.position))
            return GameApiStatus::WrongMove;
        
        Bomb::Ptr bomb = PlaceBombObject(move_data.position, player.login);
        ++state_->move_number;
        events_wrapper_->AddEvent(BombEvent({{{state_->move_number, player.actor_id, PLAYER_PLACE_BOMB}, bomb->position}, bomb->actor_id}));
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

        player1().position.x = 4;
        player1().position.y = 1;

        player2().position.x = 3;
        player2().position.y = 6;

        player1().actor_id = GetId();
        player2().actor_id = GetId();
        
        std::vector<Position> walls = {
            {0,2}, {3,2}, 
            {4,3}, {6,3},
            {1,4}, {3,4},
            {4,5}, {7,5}
        };
        for(auto& pair : walls){
            terrain().push_back(std::make_shared<Obstacle>(pair, Obstacle::Type::Wall, GetId()));
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
            return events_wrapper_->AddEvent(EmptyEvent({state_->move_number, scoreboard_[0]->actor_id, PLAYER_WON}));
        nowTurn() = nowTurn() == player1().login? player2().login : player1().login;
    }

    bool Session::ValidCell(Position position){
        if (state_->map_size.height <= position.y || state_->map_size.width <= position.x){
            spdlog::warn("cell is invalid: out of map size [{},{}]. map is {}x{}", position.x, position.y, state_->map_size.width,state_->map_size.height);
            return false;
        }

        auto it = std::find_if(terrain().begin(), terrain().end(), 
            [&](Obstacle::Ptr v){return v->position.x == position.x && v->position.y == position.y;});
        if(it != terrain().end()){
            spdlog::warn("cell is invalid: on the obstacle [{},{}]", position.x, position.y);
            return false;
        }
        
        if(player1().position.x == position.x && player1().position.y == position.y ||
           player2().position.x == position.x && player2().position.y == position.y){
            spdlog::warn("cell is invalid: on the other player [{},{}]", position.x, position.y);
            return false;
        }

        return true;
    }

    Bomb::Ptr Session::PlaceBombObject(Position position, Object::OwnerType login) {
        Bomb::Ptr obj = std::make_shared<Bomb>(login, GetId(), Bomb::Methods{
            [&](ActorId actor_id){
                RemoveObject(actor_id);
            },
            [&](Position position){
                Explode(position);
            },
        });
        obj->Place(position);
        objects().emplace_back(obj);
        return obj;
    }
    Gun::Ptr Session::PlaceGunObject(Position position, Direction direction, Object::OwnerType login) {
        Gun::Ptr obj = std::make_shared<Gun>(login, GetId(), Gun::Methods{
            [&](ActorId actor_id){
                RemoveObject(actor_id);
            },
            [&](Gun::Ptr gun){
                ShootBullet(gun);
            },
        });
        obj->Place(position, direction);
        objects().emplace_back(obj);
        return obj;
    }

    Bullet::Ptr Session::PlaceBulletObject(Position position, Direction direction, Object::OwnerType login) {
        Bullet::Ptr obj = std::make_shared<Bullet>(login, GetId(), Bullet::Methods{
            [&](ActorId actor_id){
                RemoveObject(actor_id);
            },
        });
        obj->Place(position, direction);
        objects().emplace_back(obj);
        return obj;
    }

    void Session::RemoveObject(ActorId actor_id) {
        objects().erase(std::find_if(objects().begin(), objects().end(), [&](Object::Ptr obj){return obj->actor_id == actor_id;}));
    }

    void Session::Explode(Position position) {
        for(Dimention x = std::max(0, int(position.x) - 1); x <= std::min(state_->map_size.width - 1, position.x + 1); ++x){
            for(Dimention y = std::max(0, int(position.y) - 1); y <= std::min(state_->map_size.height - 1, position.y + 1); ++y){
                if(player1().position.x == x && player1().position.y == y){
                    return FinishSession(false);
                }
                if(player2().position.x == x && player2().position.y == y){
                    return FinishSession(true);
                }
            }
        }
    }

    void Session::ShootBullet(Gun::Ptr gun){
        PlaceBulletObject(gun->position, gun->direction, gun->owner);
    }

}