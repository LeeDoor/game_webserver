#pragma once
#include <memory>

namespace game_manager{
    class State;
    class IStateRelated{
    public:
        void SetState(std::shared_ptr<State> state){
            state_ = state;
        }
    protected:
        std::shared_ptr<State> state_;
    };
}