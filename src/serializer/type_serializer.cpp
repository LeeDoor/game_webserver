#include "type_serializer.hpp"
#include "spdlog/spdlog.h"
#include "bomb.hpp"
#include "gun.hpp"
#include "bullet.hpp"

namespace game_manager{
    void to_json(json& j, const MoveData& v) {
        j = json{
            {"move_type", v.move_type}, 
            {"position", v.position}, 
            {"direction", v.direction}
        };
    }
    void from_json(const json& j, MoveData& v) {
        v = MoveData{};
        j.at("move_type").get_to(v.move_type);
        if(j.contains("position"))
            j.at("position").get_to(v.position);
        if(j.contains("direction"))
            j.at("direction").get_to(v.direction);
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