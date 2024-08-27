#include "player.hpp"

namespace game_manager{
    Player::Player() : Player({0,0}, 0, "") {}
    Player::Player(Position position, ActorId actor_id, Login login) 
        : Player(position, actor_id, login, {}){}

    Player::Player(Position position, ActorId actor_id, Login login, DieFunc&& die) 
        : IPlaceable(actor_id), login(login), die_(std::move(die)){
        Place(position);
    }
    bool Player::operator == (const Player& other) const {
        return IPlaceable::operator==(other) && login == other.login;
    }

    EventListWrapper::Vec Player::Die(int move_number){
        die_();
        return {EmptyEvent({move_number, actor_id, PLAYER_DEAD})};
    }
}