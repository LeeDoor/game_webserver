#pragma once
#include <memory>
namespace game_manager{
    class State;
    class Player;
    class IState {
    public:
        virtual void Init(const um::Login& login1, const um::Login& login2) = 0;
        virtual std::shared_ptr<const State> GetState() = 0;
        virtual void SetState(State&& state) = 0;
        virtual std::shared_ptr<Player> GetCurrentPlayer() = 0;
    };
}