#pragma once
#include "session_api_validator.hpp"

namespace game_manager{
    class SessionApiBuilder{
    public:
        /// @brief prepares product pointer for building
        SessionApiBuilder();

        /// @brief makes move independent of his turn now.
        SessionApiBuilder& SetMoveIndependent();

        /// @brief sets type of move's spread
        SessionApiBuilder& SetCellSpread(SessionApiValidator::CellSpread::Type type);
        
        /// @brief defines if player can move using current cell depends on other
        /// objects on it.
        /// @param restrictions structure with some fields for every type of object
        /// on map. set true if player can move on cell with this object. 
        SessionApiBuilder& SetCellRestrictor(SessionApiValidator::CellRestrictor::Restrictions restrictions);

        /// @brief sets the distance of our move. distance measures in all directions including player's cell
        /// (if restrictor allows). 
        /// @param distance distance in cells
        SessionApiBuilder& SetDistanceValidator(Dimention distance);

        /// @brief sets cell-targeted parameters to default values. 
        /// they can (and some of them should) be modified.
        /// should be called before other Set- functions.
        SessionApiBuilder& MakeTargetedToCell();

        /// @brief gets built product and prepares pointer for a new building.
        /// @return returns unique pointer with moved owner to a new product
        SessionApiValidator GetProduct();

    private:
        SessionApiValidator product_;
    };  
}