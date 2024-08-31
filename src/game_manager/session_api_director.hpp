#pragma once
#include "session_api_builder.hpp"

namespace game_manager{
    class SessionApiDirector {
        using SpreadType = SessionApiValidator::CellSpread::Type;
    public:
        SessionApiDirector();
        SessionApiValidator BuildWalk();
        SessionApiValidator BuildResign();
        SessionApiValidator BuildPlaceBomb();
        SessionApiValidator BuildPlaceGun();

    private:
        SessionApiBuilder builder_;
    };
}