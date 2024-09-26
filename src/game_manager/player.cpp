#include "player.hpp"
#include "session.hpp"
#include "bullet.hpp"

namespace game_manager{
    Player::Player() : Player({0,0}, 0, "") {}
    Player::Player(Position position, ActorId actor_id, Id login) 
        : IPlaceable(actor_id), login(login){
        Place(position);
    }
    bool Player::operator == (const Player& other) const {
        return IPlaceable::operator==(other) && login == other.login;
    }

    EventListWrapper::Vec Player::Die(){
        interactor_->FinishSession(actor_id == 1);
        return {EmptyEvent({actor_id, PLAYER_DEAD})};
    }

    EventsType Player::InteractWithBullet(Bullet::Ptr bullet) {
        return Die();
    }
}