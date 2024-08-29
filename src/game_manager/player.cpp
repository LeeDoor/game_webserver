#include "player.hpp"
#include "state.hpp"

namespace game_manager{
    Player::Player() : Player({0,0}, 0, "") {}
    Player::Player(Position position, ActorId actor_id, Login login) 
        : IPlaceable(actor_id), login(login){
        Place(position);
    }
    bool Player::operator == (const Player& other) const {
        return IPlaceable::operator==(other) && login == other.login;
    }

    EventListWrapper::Vec Player::Die(int move_number){
        state_->FinishSession(actor_id == 1);
        return {EmptyEvent({move_number, actor_id, PLAYER_DEAD})};
    }
}