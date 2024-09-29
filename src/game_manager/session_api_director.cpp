#include "session_api_director.hpp"
#include "const_variables.hpp"

namespace game_manager{
    SessionApiDirector::SessionApiDirector(){}

    SessionApiValidator SessionApiDirector::BuildWalk(){
        auto res = builder_
            .MakeTargetedToCell()
            .SetCellSpread(SpreadType::Axial)
            .SetDistanceValidator(1)
            .GetProduct();
        return std::move(res);
    }
    SessionApiValidator SessionApiDirector::BuildResign(){
        auto res = builder_
            .SetMoveIndependent()
            .GetProduct();
        return std::move(res);
    }
    SessionApiValidator SessionApiDirector::BuildPlaceBomb(){
        auto res = builder_
            .MakeTargetedToCell()
            .SetCellSpread(SpreadType::Square)
            .SetDistanceValidator(BOMB_PLACE_RADIUS)
            .GetProduct();
        return std::move(res);
    }
    SessionApiValidator SessionApiDirector::BuildPlaceGun(){
        auto res = builder_
            .MakeTargetedToCell()
            .SetCellSpread(SpreadType::Square)
            .SetDistanceValidator(GUN_PLACE_RADIUS)
            .GetProduct();
        return std::move(res);
    }
}