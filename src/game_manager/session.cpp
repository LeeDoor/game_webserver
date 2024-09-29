#include "session.hpp"
#include "spdlog/spdlog.h"
#include "event_manager.hpp"
#include "nlohmann/json.hpp"
#include "type_serializer.hpp"
#include "const_variables.hpp"

#define PLAYER_WON "player_won"

namespace game_manager {
    std::map<MoveType, SessionApiValidator> Session::api_validator_ = {};
    Session::Session() :
        events_wrapper_(std::make_shared<EventListWrapper>()),
        state_(std::make_shared<State>()){
        events_wrapper_->SetMoveNumber(state_->move_number);
    }


    void Session::UpdateStatePointers(){
        std::for_each(state_->players.begin(), state_->players.end(), [&](Player::Ptr a){a->SetInteractor(shared_from_this());});
        std::for_each(state_->objects.begin(), state_->objects.end(), [&](Object::Ptr a){a->SetInteractor(shared_from_this());});
        std::for_each(state_->terrain.begin(), state_->terrain.end(), [&](Obstacle::Ptr a){a->SetInteractor(shared_from_this());});
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

    void Session::Init(const Player::Id& id1, const Player::Id& id2){
        state_->players = {
            std::make_shared<Player>(gm::Position{4, 1}, GetId(), id1),
            std::make_shared<Player>(gm::Position{3, 6}, GetId(), id2),
        };
        
        std::vector<Position> walls = {
            {0,2}, {3,2}, 
            {4,3}, {6,3},
            {1,4}, {3,4},
            {4,5}, {7,5}
        };
        for(auto& pair : walls){
            state_->terrain.push_back(std::make_shared<Obstacle>(pair, Obstacle::Type::Wall, GetId()));
        }

        state_->now_turn = id1;
        state_->map_size = {8,8};
        UpdateStatePointers(); 
    }
    EventListWrapper::CPtr Session::GetEvents() {
        return events_wrapper_;
    }

    std::shared_ptr<const State> Session::GetState() {
        return state_;
    }
    void Session::SetState(State::Ptr state) {
        state_ = state;
        UpdateStatePointers();
        events_wrapper_->Clear();
        id_counter_ = 2;
    }
    std::shared_ptr<Player> Session::GetCurrentPlayer() {
        return player1()->login == state_->now_turn ? player1() : player2();
    }
    bool Session::HasPlayer(const Player::Id& id) {
        return id == player1()->login || id == player2()->login;
    }
    std::optional<Results> Session::GetResults(){
        if(scoreboard_.empty())
            return std::nullopt;
        return Results{scoreboard_[0].lock()->login, scoreboard_[1].lock()->login};
    } 
    GameApiStatus Session::ApiMove(Player::Id id, MoveData md) {
        Player::Ptr player;
        if(id == player1()->login)
            player = player1();
        else player = player2();
        if(auto status = api_validator_.at(md.move_type)(state_, player, md); status != GameApiStatus::Ok)
            return status;

        IncreaseMoveNumber();
        switch(md.move_type) {
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
    void Session::ApiWalk(Player::Ptr player, MoveData md) {
        player->position.x = md.position.x;
        player->position.y = md.position.y;
        events_wrapper_->AddEvent(WalkEvent({{player->actor_id, PLAYER_WALK}, player->position}));
    }
    void Session::ApiResign(Player::Ptr player, MoveData md) {
        FinishSession(player1()->actor_id != player->actor_id);
        events_wrapper_->AddEvent(EmptyEvent({player->actor_id, PLAYER_RESIGN}));
    }
    void Session::ApiPlaceBomb(Player::Ptr player, MoveData md) {
        Bomb::Ptr bomb = PlaceBombObject(md.position, player->login);
        events_wrapper_->AddEvent(BombEvent({{{player->actor_id, PLAYER_PLACE_BOMB}, bomb->position}, bomb->actor_id}));
    }
    void Session::ApiPlaceGun(Player::Ptr player, MoveData md) {
        Gun::Ptr gun = PlaceGunObject(md.position, md.direction, player->login);
        events_wrapper_->AddEvent(GunEvent({{{{player->actor_id, PLAYER_PLACE_GUN}, gun->position}, gun->actor_id}, gun->direction}));
    }

    void Session::AfterMove(){
        std::list<Object::Ptr>::iterator it, prev; // first and second iterators
        if(state_->objects.size() != 0){
            do{
                prev = state_->objects.begin(); // eventing first while it is changing
                EventsType events = (*prev)->UpdateTick(); // if it is, we are eventing first 
                events_wrapper_->AddEvents(std::move(events)); // element again
            } while (prev != state_->objects.begin() && !state_->objects.empty());
        }
        if(state_->objects.size() != 0){
            it = state_->objects.begin();
            ++it;
            while(it != state_->objects.end()){
                if(!scoreboard_.empty()) break;

                EventsType events = (*it)->UpdateTick();
                events_wrapper_->AddEvents(std::move(events));

                if(std::next(prev) != it) { // when we check if there were any changes with current element
                    it = std::next(prev);
                }
                else{
                    prev = it;
                    ++it;
                }
            }
        }
        if(!scoreboard_.empty())
            return events_wrapper_->AddEvent(EmptyEvent({scoreboard_[0].lock()->actor_id, PLAYER_WON}));
        state_->now_turn = state_->now_turn == player1()->login? player2()->login : player1()->login;
    }

    void Session::IncreaseMoveNumber() {
        ++state_->move_number;
        events_wrapper_->SetMoveNumber(state_->move_number);
    }

    bool Session::ValidCell(Position position){
        if (state_->map_size.height <= position.y || state_->map_size.width <= position.x){
            spdlog::warn("cell is invalid: out of map size [{},{}]. map is {}x{}", position.x, position.y, state_->map_size.width, state_->map_size.height);
            return false;
        }

        auto it = std::find_if(state_->terrain.begin(), state_->terrain.end(), 
            [&](Obstacle::Ptr v){return v->position.x == position.x && v->position.y == position.y;});
        if(it != state_->terrain.end()){
            spdlog::warn("cell is invalid: on the obstacle [{},{}]", position.x, position.y);
            return false;
        }
        
        if(player1()->position == position || player2()->position == position){
            spdlog::warn("cell is invalid: on the other player [{},{}]", position.x, position.y);
            return false;
        }

        return true;
    }

    void Session::FinishSession(bool firstWinner) {
        if(firstWinner){
            scoreboard_.push_back(state_->players.front());
            scoreboard_.push_back(state_->players.back());
        }
        else{
            scoreboard_.push_back(state_->players.back());
            scoreboard_.push_back(state_->players.front());
        }
    }

    Bomb::Ptr Session::PlaceBombObject(Position position, Object::OwnerType login) {
        Bomb::Ptr obj = std::make_shared<Bomb>(login, GetId());
        obj->Place(position);
        obj->SetInteractor(shared_from_this());
        state_->objects.emplace_back(obj);
        return obj;
    }
    Gun::Ptr Session::PlaceGunObject(Position position, Direction direction, Object::OwnerType login) {
        Gun::Ptr obj = std::make_shared<Gun>(login, GetId());
        obj->Place(position, direction);
        obj->SetInteractor(shared_from_this());
        state_->objects.emplace_back(obj);
        return obj;
    }

    Bullet::Ptr Session::PlaceBulletObject(Position position, Direction direction, Object::OwnerType login) {
        Bullet::Ptr obj = std::make_shared<Bullet>(login, GetId());
        obj->Place(position, direction);
        obj->SetInteractor(shared_from_this());
        state_->objects.emplace_back(obj);
        return obj;
    }

    void Session::RemoveObject(ActorId actor_id) {
        state_->objects.erase(std::find_if(state_->objects.begin(), state_->objects.end(), [&](Object::Ptr obj){return obj->actor_id == actor_id;}));
    }

    void Session::Explode(Position position) {
        for(Dimention x = std::max(0, position.x -BOMB_EXPLODE_RADIUS); x <= std::min(state_->map_size.width - 1, position.x +BOMB_EXPLODE_RADIUS); ++x){
            for(Dimention y = std::max(0, position.y -BOMB_EXPLODE_RADIUS); y <= std::min(state_->map_size.height - 1, position.y +BOMB_EXPLODE_RADIUS); ++y){
                Position pos {x,y};
                if(player1()->position == pos){
                    player1()->Die();
                    return;
                }
                if(player2()->position == pos){
                    player2()->Die();
                    return;
                }
            }
        }
    } 

    std::optional<std::list<IPlaceable::Ptr>> Session::CollisionsOnCell(Bullet::Ptr bullet) {
        Position& position = bullet->position;
        if(position.x >= state_->map_size.width || position.y >= state_->map_size.height ||
           position.x < 0 || position.y < 0)
            return std::nullopt;

        std::list<IPlaceable::Ptr> collisions;
        std::copy_if(state_->terrain.begin(), state_->terrain.end(), std::back_inserter(collisions), 
            [&](Obstacle::Ptr o){ return o->position == position; });

        std::copy_if(state_->objects.begin(), state_->objects.end(), std::back_inserter(collisions), 
            [&](Object::Ptr o){ return o->position == position && o->actor_id != bullet->actor_id; });
        
        std::copy_if(state_->players.begin(), state_->players.end(), std::back_inserter(collisions), 
            [&](Player::Ptr p){ return p->position == position; });
        
        return collisions;
    }
}