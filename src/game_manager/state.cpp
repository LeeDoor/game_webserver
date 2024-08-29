#include "state.hpp"
#include "spdlog/spdlog.h"
#include "event_manager.hpp"

#define PLAYER_WON "player_won"

namespace game_manager {
    State::State() :
        events_wrapper_(std::make_shared<EventListWrapper>()){}

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
        std::for_each(players.begin(), players.end(), [&](Player::Ptr a){a->SetState(shared_from_this());});
        std::for_each(objects.begin(), objects.end(), [&](Object::Ptr a){a->SetState(shared_from_this());});
        std::for_each(terrain.begin(), terrain.end(), [&](Obstacle::Ptr a){a->SetState(shared_from_this());});
    }

    void State::Init(const um::Login& login1, const um::Login& login2){
        players = {
            std::make_shared<Player>(gm::Position{4, 1}, GetId(), login1),
            std::make_shared<Player>(gm::Position{3, 6}, GetId(), login2),
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

        now_turn = login1;
        map_size = {15,15};
        UpdateStatePointers(); 
    }

    void State::AfterMove(){
        for(size_t i = 0, len = objects.size(); i < objects.size(); ++i){
            auto it = objects.begin();
            std::advance(it, i);
            if(!scoreboard_.empty()) break;

            Object::EventsType events = (*it)->UpdateTick(move_number);
            events_wrapper_->AddEvents(std::move(events));
            if(len > objects.size()) --i;
            len = objects.size();
        }
        if(!scoreboard_.empty())
            return events_wrapper_->AddEvent(EmptyEvent({move_number, scoreboard_[0].lock()->actor_id, PLAYER_WON}));
        now_turn = now_turn == player1()->login? player2()->login : player1()->login;
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
        obj->SetState(shared_from_this());
        objects.emplace_back(obj);
        return obj;
    }
    Gun::Ptr State::PlaceGunObject(Position position, Direction direction, Object::OwnerType login) {
        Gun::Ptr obj = std::make_shared<Gun>(login, GetId());
        obj->Place(position, direction);
        obj->SetState(shared_from_this());
        objects.emplace_back(obj);
        return obj;
    }

    Bullet::Ptr State::PlaceBulletObject(Position position, Direction direction, Object::OwnerType login) {
        Bullet::Ptr obj = std::make_shared<Bullet>(login, GetId());
        obj->Place(position, direction);
        obj->SetState(shared_from_this());
        objects.emplace_back(obj);
        return obj;
    }

    void State::RemoveObject(ActorId actor_id) {
        objects.erase(std::find_if(objects.begin(), objects.end(), [&](Object::Ptr obj){return obj->actor_id == actor_id;}));
    }

    void State::Explode(Position position) {
        for(Dimention x = std::max(0, int(position.x) - 1); x <= std::min(map_size.width - 1, position.x + 1); ++x){
            for(Dimention y = std::max(0, int(position.y) - 1); y <= std::min(map_size.height - 1, position.y + 1); ++y){
                Position pos {x,y};
                if(player1()->position == pos){
                    player1()->Die(move_number);
                    return;
                }
                if(player2()->position == pos){
                    player2()->Die(move_number);
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