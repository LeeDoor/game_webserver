#include "type_serializer.hpp"
#include "spdlog/spdlog.h"
#include "bomb.hpp"
#include "gun.hpp"
#include "bullet.hpp"

namespace game_manager{
    void to_json(json& j, const State& v) {
        j = json{
            {"state", "playing"}, 
            {"players", v.players}, 
            {"objects", v.objects}, 
            {"terrain", v.terrain}, 
            {"now_turn", v.now_turn},
            {"map_size", v.map_size},
            {"move_number", v.move_number},
        };
    }
    void from_json(const json& j, State& v) {
        j.at("players").get_to(v.players);
        j.at("objects").get_to(v.objects);
        j.at("terrain").get_to(v.terrain);
        j.at("now_turn").get_to(v.now_turn);
        j.at("map_size").get_to(v.map_size);
        j.at("move_number").get_to(v.move_number);
    }
}

namespace session_manager{
    void to_json(json& j, const PublicSessionData& v) {
        j = json{
            {"state", "finished"}, 
            {"id", v.id}, 
            {"winner", v.winner?*v.winner:"null"}, 
            {"loser", v.loser?*v.loser:"null"}
        };
    }
    void from_json(const json& j, PublicSessionData& v) {
        j.at("id").get_to(v.id);
        um::Login login;
        j.at("winner").get_to(login);
        if(login == "null")
            v.winner = std::nullopt;
        else
            v.winner = login;
        j.at("loser").get_to(login);
        if(login == "null")
            v.loser = std::nullopt;
        else
            v.loser = login;
    }
}