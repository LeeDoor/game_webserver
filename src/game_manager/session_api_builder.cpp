#include "session_api_builder.hpp"

namespace game_manager{
    SessionApiBuilder::SessionApiBuilder(){}

    SessionApiBuilder& SessionApiBuilder::SetMoveIndependent() {
        product_.SetMoveDependent({false});
        return *this;
    }
    SessionApiBuilder& SessionApiBuilder::SetCellSpread(SessionApiValidator::CellSpread::Type type) {
        product_.SetCellSpread({type});
        return *this;
    }
    SessionApiBuilder& SessionApiBuilder::SetCellRestrictor(SessionApiValidator::CellRestrictor::Restrictions restrictions) {
        product_.SetCellRestrictor({restrictions});
        return *this;
    }
    SessionApiBuilder& SessionApiBuilder::SetDistanceValidator(Dimention distance) {
        product_.SetDistanceValidator({distance});
        return *this;
    }
    SessionApiBuilder& SessionApiBuilder::MakeTargetedToCell() {
        SetCellRestrictor({});
        SetDistanceValidator(1);
        return *this;
    }
    SessionApiValidator SessionApiBuilder::GetProduct() {
        SessionApiValidator buff = std::move(product_);
        product_ = {};
        return std::move(buff);
    }
}