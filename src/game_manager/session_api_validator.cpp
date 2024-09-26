#include "session_api_validator.hpp"
#include "session.hpp"

namespace game_manager{
    SessionApiValidator::SessionApiValidator(){}

    void SessionApiValidator::SetMoveDependent(MoveDependent&& md) {
        move_dependent_ = std::move(md);
    }
    void SessionApiValidator::SetCellSpread(CellSpread&& cs) {
        cell_spread_ = std::move(cs);
    }
    void SessionApiValidator::SetCellRestrictor(CellRestrictor&& cr) {
        cell_restrictor_ = std::move(cr);
    }
    void SessionApiValidator::SetDistanceValidator(DistanceValidator&& dv) {
        distance_validator_ = std::move(dv);
    }

    SessionApiValidator::MoveDependent::MoveDependent(bool depends_move)
        : depends_move_(depends_move){}
    bool SessionApiValidator::MoveDependent::operator()(const um::Login& now_turn, const um::Login& actor) const{
        return !depends_move_ || now_turn == actor;
    }

    SessionApiValidator::CellSpread::CellSpread(Type type)
        : type_(type){}
    bool SessionApiValidator::CellSpread::operator ()(Position player_pos, Position cell_pos) const{
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
        default:
            throw std::runtime_error("CellSpread::Type behavior not defined");
        }
        return false;
    }

    SessionApiValidator::CellRestrictor::CellRestrictor(Restrictions rest)
        : restrictions_(rest){}
    bool SessionApiValidator::CellRestrictor::operator ()(Session::Ptr state, Position cell_pos) const{
        if(!restrictions_.object){
            auto obj_iter = std::find_if(state->objects.begin(), state->objects.end(), [&](Object::Ptr obj){
                return obj->position == cell_pos;
            });
            if(obj_iter != state->objects.end())
                return false;
        }
        if(!restrictions_.player){
            auto p_iter = std::find_if(state->players.begin(), state->players.end(), [&](Player::Ptr p){
                return p->position == cell_pos;
            });
            if(p_iter != state->players.end())
                return false;
        }
        if(!restrictions_.wall){
            auto o_iter = std::find_if(state->terrain.begin(), state->terrain.end(), [&](Obstacle::Ptr o){
                return o->type == Obstacle::Type::Wall && o->position == cell_pos;
            });
            if(o_iter != state->terrain.end())
                return false;
        }
        return true;
    }

    SessionApiValidator::DistanceValidator::DistanceValidator(Dimention distance)
        : distance_(distance){}
    bool SessionApiValidator::DistanceValidator::operator()(Position player_pos, Position cell_pos) const {
        return std::abs(player_pos.x - cell_pos.x) <= distance_ &&
               std::abs(player_pos.y - cell_pos.y) <= distance_;
    }

    GameApiStatus SessionApiValidator::operator()(Session::Ptr state, Player::Ptr player, MoveData md) {
        if(!move_dependent_(state->now_turn, player->login)) return GameApiStatus::NotYourMove;
        if(cell_spread_) {
            if(!(*cell_spread_)(player->position, md.position)) return GameApiStatus::WrongMove;
            if(!(*cell_restrictor_)(state, md.position)) return GameApiStatus::WrongMove;
            if(!(*distance_validator_)(player->position, md.position)) return GameApiStatus::WrongMove;
        }
        return GameApiStatus::Ok;
    }
}