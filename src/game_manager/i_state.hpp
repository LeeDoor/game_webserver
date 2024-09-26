#pragma once
#include <memory>
namespace game_manager{
    class Session;
    class Player;
    class IState {
    public:
        virtual void Init(const Player::Id& login1, const Player::Id& login2) = 0;
        virtual std::shared_ptr<const Session> GetState() = 0;
        virtual void SetState(std::shared_ptr<Session> state) = 0;
        virtual std::shared_ptr<Player> GetCurrentPlayer() = 0;
    };
}