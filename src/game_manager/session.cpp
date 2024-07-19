#include "session.hpp"

namespace game_manager{

    Session::Session(dm::Uuid&& player1, dm::Uuid&& player2) 
    :player1_(player1), player2_(player2){}

    bool Session::HasPlayer(const dm::Uuid& uuid){
        return player1_ == uuid || player2_ == uuid;
    }

    bool Session::Ping(const dm::Uuid& player_id){
        return true;
    }
}