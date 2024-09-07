#pragma once
#include <algorithm>
#include "user.hpp"
#include "position.hpp"
#include "state.hpp"
#include "move_data.hpp"
#include "game_api_status.hpp"

namespace game_manager{
    class SessionApiValidator {
    public:
        SessionApiValidator();

        class MoveDependent {
        public:
            MoveDependent(bool depends_move = true);
            bool operator()(const um::Login& now_turn, const um::Login& actor) const;
        private:
            bool depends_move_;
        };
        class CellSpread {
        public:
            enum Type {
                Axial, AxialDiagonal, Square
            };
            CellSpread(Type type);
            bool operator ()(Position player_pos, Position cell_pos) const;
        private:
            Type type_;
        };

        class CellRestrictor{
        public:
            struct Restrictions {
                bool player = false;
                bool object = false;
                bool wall = false;
            };
            CellRestrictor(Restrictions rest);
            bool operator ()(State::Ptr state, Position cell_pos) const;
        private:
            Restrictions restrictions_;
        };

        class DistanceValidator {
        public:
            DistanceValidator(Dimention distance);
            bool operator()(Position player_pos, Position cell_pos) const;
        private:
            Dimention distance_;
        };

        void SetMoveDependent(MoveDependent&& md);
        void SetCellSpread(CellSpread&& cs);
        void SetCellRestrictor(CellRestrictor&& cr);
        void SetDistanceValidator(DistanceValidator&& dv);

        GameApiStatus operator()(State::Ptr state, Player::Ptr player, MoveData md);
    private:
        MoveDependent move_dependent_;
        std::optional<CellSpread> cell_spread_;
        std::optional<CellRestrictor> cell_restrictor_;
        std::optional<DistanceValidator> distance_validator_;
    };
}
