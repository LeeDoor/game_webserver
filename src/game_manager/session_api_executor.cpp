#include "session_api_executor.hpp"

namespace game_manager{
    SessionApiExecutor::SessionApiExecutor(){}

    void SessionApiExecutor::SetMoveDependant(MoveDependant&& md) {
        move_dependant_ = std::move(md);
    }
    void SessionApiExecutor::SetCellSpread(CellSpread&& cs) {
        cell_spread_ = std::move(cs);
    }
    void SessionApiExecutor::SetCellRestrictor(CellRestrictor&& cr) {
        cell_restrictor_ = std::move(cr);
    }
    void SessionApiExecutor::SetDistanceValidator(DistanceValidator&& dv) {
        distance_validator_ = std::move(dv);
    }

    SessionApiExecutor::MoveDependant::MoveDependant(bool depends_move)
        : depends_move_(depends_move){}
    bool SessionApiExecutor::MoveDependant::operator()(const um::Login& now_turn, const um::Login& actor) const{
        return !depends_move_ || now_turn == actor;
    }

    SessionApiExecutor::CellSpread::CellSpread(Type type)
        : type_(type){}
    bool SessionApiExecutor::CellSpread::operator ()(Position player_pos, Position cell_pos) const{
        switch(type_){
        case Type::AxialDiagonal:
            // diagonals
            if( cell_pos.y - cell_pos.x == player_pos.y - player_pos.x || 
                cell_pos.y + cell_pos.x == player_pos.y + player_pos.x)
                return true;
            // no break
        case Type::Axial:
            if(cell_pos.x == player_pos.x || cell_pos.y == player_pos.y)
                return true;
            break;
        case Type::Square:
            return true;
        }
        throw std::runtime_error("CellSpread::Type behavior not defined");
    }

    SessionApiExecutor::CellRestrictor::CellRestrictor(Restrictions rest)
        : restrictions_(rest){}
    bool SessionApiExecutor::CellRestrictor::operator ()(State::Ptr state, Position cell_pos) const{
        if(restrictions_.object){
            auto obj_iter = std::find_if(state->objects.begin(), state->objects.end(), [&](Object::Ptr obj){
                return obj->position == cell_pos;
            });
            if(obj_iter != state->objects.end())
                return false;
        }
        if(restrictions_.player){
            auto p_iter = std::find_if(state->players.begin(), state->players.end(), [&](Player::Ptr p){
                return p->position == cell_pos;
            });
            if(p_iter != state->players.end())
                return false;
        }
        if(restrictions_.wall){
            auto o_iter = std::find_if(state->terrain.begin(), state->terrain.end(), [&](Obstacle::Ptr o){
                return o->type == Obstacle::Type::Wall && o->position == cell_pos;
            });
            if(o_iter != state->terrain.end())
                return false;
        }
        return true;
    }

    SessionApiExecutor::DistanceValidator::DistanceValidator(Dimention distance)
        : distance_(distance){}
    bool SessionApiExecutor::DistanceValidator::operator()(Position player_pos, Position cell_pos) const {
        if( std::abs(player_pos.x - cell_pos.x) <= distance_ &&
            std::abs(player_pos.y - cell_pos.y) <= distance_)
            return true;
    }
}