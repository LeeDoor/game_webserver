#include "state.hpp"
#include "spdlog/spdlog.h"
#include "event_manager.hpp"
#include "nlohmann/json.hpp"
#include "type_serializer.hpp"

#define PLAYER_WON "player_won"

namespace game_manager {
    std::map<MoveType, SessionApiValidator> State::api_validator_ = {};
    State::State() :
        events_wrapper_(std::make_shared<EventListWrapper>()){
        events_wrapper_->SetMoveNumber(move_number);
    }

    State::State(State&& other) {
        players = std::move(other.players);
        objects = std::move(other.objects);
        terrain = std::move(other.terrain);
        now_turn = std::move(other.now_turn);
        map_size = std::move(other.map_size);
        move_number = std::move(other.move_number);
    }

    State& State::operator=(State&& other) {
        players = std::move(other.players);
        objects = std::move(other.objects);
        terrain = std::move(other.terrain);
        now_turn = std::move(other.now_turn);
        map_size = std::move(other.map_size);
        move_number = std::move(other.move_number);
        return *this;
    }

    template<template <typename> typename Container, typename Type>
    bool comp_cont(Container<std::shared_ptr<Type>> c1, Container<std::shared_ptr<Type>> c2){
        return std::equal(c1.begin(), c1.end(), c2.begin(), [](std::shared_ptr<Type> a, std::shared_ptr<Type> b){ return *a == *b; });
    }
    template<template <typename> typename Container, typename Type>
    bool comp_cont_vtable(Container<std::shared_ptr<Type>> c1, Container<std::shared_ptr<Type>> c2){
        return std::equal(c1.begin(), c1.end(), c2.begin(), [](std::shared_ptr<Type> a, std::shared_ptr<Type> b){ return *a == b; });
    }

    bool State::operator==(const State& s) const {
        bool a = comp_cont(players, s.players);
        a = a && comp_cont(terrain, s.terrain);
        a = a && now_turn == s.now_turn;
        a = a && map_size == s.map_size;
        a = a && comp_cont_vtable(objects, s.objects);
        return a;
    }

    void State::UpdateStatePointers(){
        std::for_each(players.begin(), players.end(), [&](Player::Ptr a){a->SetInteractor(shared_from_this());});
        std::for_each(objects.begin(), objects.end(), [&](Object::Ptr a){a->SetInteractor(shared_from_this());});
        std::for_each(terrain.begin(), terrain.end(), [&](Obstacle::Ptr a){a->SetInteractor(shared_from_this());});
    }
    void State::InitApi() {
        SessionApiDirector director;
        api_validator_ = {
            {MoveType::Walk, director.BuildWalk()},
            {MoveType::Resign, director.BuildResign()},
            {MoveType::PlaceBomb, director.BuildPlaceBomb()},
            {MoveType::PlaceGun, director.BuildPlaceGun()}
        };
    }

    std::string State::tojson() const{
        nlohmann::json j = nlohmann::json{
            {"state", "playing"}, 
            {"players", players}, 
            {"objects", objects}, 
            {"terrain", terrain}, 
            {"now_turn", now_turn},
            {"map_size", map_size},
            {"move_number", move_number},
        };
        return j.dump();
    }
    void State::fromjson(const std::string& str) {
        nlohmann::json j = nlohmann::json::parse(str);
        j.at("players").get_to(players);
        j.at("objects").get_to(objects);
        j.at("terrain").get_to(terrain);
        j.at("now_turn").get_to(now_turn);
        j.at("map_size").get_to(map_size);
        j.at("move_number").get_to(move_number);
    }

    void State::Init(const um::Uuid& id1, const um::Uuid& id2){
        players = {
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
            terrain.push_back(std::make_shared<Obstacle>(pair, Obstacle::Type::Wall, GetId()));
        }

        now_turn = id1;
        map_size = {15,15};
        UpdateStatePointers(); 
    }
    EventListWrapper::CPtr State::GetEvents() {
        return events_wrapper_;
    }

    std::shared_ptr<const State> State::GetState() {
        return shared_from_this();
    }
    void State::SetState(State&& state) {
        state.UpdateStatePointers();
        state.events_wrapper_->Clear();
        state.id_counter_ = 2;
        *this = std::move(state);
    }
    std::shared_ptr<Player> State::GetCurrentPlayer() {
        return player1()->login == now_turn ? player1() : player2();
    }
    bool State::HasPlayer(um::Uuid login) {
        return login == player1()->login || login == player2()->login;
    }
    std::optional<Results> State::GetResults(){
        if(scoreboard_.size())
            return std::nullopt;
        return Results{scoreboard_[0].lock()->login, scoreboard_[1].lock()->login};
    } 
    GameApiStatus State::ApiMove(um::Uuid uuid, MoveData md) {
        IncreaseMoveNumber();
        Player::Ptr player;
        if(uuid == player1()->login)
            player = player1();
        else if (uuid == player2()->login)
            player = player2();
        else return GameApiStatus::WrongMove;
        if(auto status = api_validator_.at(md.move_type)(shared_from_this(), player, md); status != GameApiStatus::Ok)
            return status;

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
    void State::ApiWalk(Player::Ptr player, MoveData md) {
        player->position.x = md.position.x;
        player->position.y = md.position.y;
        events_wrapper_->AddEvent(WalkEvent({{player->actor_id, PLAYER_WALK}, player->position}));
    }
    void State::ApiResign(Player::Ptr player, MoveData md) {
        FinishSession(player1()->actor_id != player->actor_id);
        events_wrapper_->AddEvent(EmptyEvent({player->actor_id, PLAYER_RESIGN}));
    }
    void State::ApiPlaceBomb(Player::Ptr player, MoveData md) {
        Bomb::Ptr bomb = PlaceBombObject(md.position, player->login);
        events_wrapper_->AddEvent(BombEvent({{{player->actor_id, PLAYER_PLACE_BOMB}, bomb->position}, bomb->actor_id}));
    }
    void State::ApiPlaceGun(Player::Ptr player, MoveData md) {
        Gun::Ptr gun = PlaceGunObject(md.position, md.direction, player->login);
        events_wrapper_->AddEvent(GunEvent({{{{player->actor_id, PLAYER_PLACE_GUN}, gun->position}, gun->actor_id}, gun->direction}));
    }

    void State::AfterMove(){
        for(size_t i = 0, len = objects.size(); i < objects.size(); ++i){
            auto it = objects.begin();
            std::advance(it, i);
            if(!scoreboard_.empty()) break;

            Object::EventsType events = (*it)->UpdateTick();
            events_wrapper_->AddEvents(std::move(events));
            if(len > objects.size()) --i;
            len = objects.size();
        }
        if(!scoreboard_.empty())
            return events_wrapper_->AddEvent(EmptyEvent({scoreboard_[0].lock()->actor_id, PLAYER_WON}));
        now_turn = now_turn == player1()->login? player2()->login : player1()->login;
    }

    void State::IncreaseMoveNumber() {
        ++move_number;
        events_wrapper_->SetMoveNumber(move_number);
    }

    bool State::ValidCell(Position position){
        if (map_size.height <= position.y || map_size.width <= position.x){
            spdlog::warn("cell is invalid: out of map size [{},{}]. map is {}x{}", position.x, position.y, map_size.width, map_size.height);
            return false;
        }

        auto it = std::find_if(terrain.begin(), terrain.end(), 
            [&](Obstacle::Ptr v){return v->position.x == position.x && v->position.y == position.y;});
        if(it != terrain.end()){
            spdlog::warn("cell is invalid: on the obstacle [{},{}]", position.x, position.y);
            return false;
        }
        
        if(player1()->position == position || player2()->position == position){
            spdlog::warn("cell is invalid: on the other player [{},{}]", position.x, position.y);
            return false;
        }

        return true;
    }

    void State::FinishSession(bool firstWinner) {
        if(firstWinner){
            scoreboard_.push_back(players.front());
            scoreboard_.push_back(players.back());
        }
        else{
            scoreboard_.push_back(players.back());
            scoreboard_.push_back(players.front());
        }
    }

    Bomb::Ptr State::PlaceBombObject(Position position, Object::OwnerType login) {
        Bomb::Ptr obj = std::make_shared<Bomb>(login, GetId());
        obj->Place(position);
        obj->SetInteractor(shared_from_this());
        objects.emplace_back(obj);
        return obj;
    }
    Gun::Ptr State::PlaceGunObject(Position position, Direction direction, Object::OwnerType login) {
        Gun::Ptr obj = std::make_shared<Gun>(login, GetId());
        obj->Place(position, direction);
        obj->SetInteractor(shared_from_this());
        objects.emplace_back(obj);
        return obj;
    }

    Bullet::Ptr State::PlaceBulletObject(Position position, Direction direction, Object::OwnerType login) {
        Bullet::Ptr obj = std::make_shared<Bullet>(login, GetId());
        obj->Place(position, direction);
        obj->SetInteractor(shared_from_this());
        objects.emplace_back(obj);
        return obj;
    }

    void State::RemoveObject(ActorId actor_id) {
        objects.erase(std::find_if(objects.begin(), objects.end(), [&](Object::Ptr obj){return obj->actor_id == actor_id;}));
    }

    void State::Explode(Position position) {
        for(Dimention x = std::max(0, position.x - 1); x <= std::min(map_size.width - 1, position.x + 1); ++x){
            for(Dimention y = std::max(0, position.y - 1); y <= std::min(map_size.height - 1, position.y + 1); ++y){
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

    std::optional<std::list<IPlaceable::Ptr>> State::CollisionsOnCell(Bullet::Ptr bullet) {
        Position& position = bullet->position;
        if(position.x >= map_size.width || position.y >= map_size.height)
            return std::nullopt;

        std::list<IPlaceable::Ptr> collisions;
        std::copy_if(terrain.begin(), terrain.end(), std::back_inserter(collisions), 
            [&](Obstacle::Ptr o){ return o->position == position; });

        std::copy_if(objects.begin(), objects.end(), std::back_inserter(collisions), 
            [&](Object::Ptr o){ return o->position == position && o->actor_id != bullet->actor_id; });
        
        std::copy_if(players.begin(), players.end(), std::back_inserter(collisions), 
            [&](Player::Ptr p){ return p->position == position; });
        
        return collisions;
    }
}