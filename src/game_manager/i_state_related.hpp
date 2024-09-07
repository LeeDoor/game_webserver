#pragma once
#include <memory>

namespace game_manager{
    class IInteractionApi;
    class IStateRelated{
    public:
        void SetInteractor(std::shared_ptr<IInteractionApi> inter){
            interactor_ = inter;
        }
    protected:
        std::shared_ptr<IInteractionApi> interactor_;
    };
}