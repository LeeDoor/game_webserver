#include "type_serializer.hpp"
#include "spdlog/spdlog.h"
#include "bomb.hpp"

namespace game_manager{
    void to_json(json& j, const State& v) {
        j = json{
            {"state", "playing"}, 
            {"players", v.players}, 
            {"objects", v.objects}, 
            {"terrain", v.terrain}, 
            {"now_turn", v.now_turn},
            {"map_size", v.map_size},
        };
    }
    void from_json(const json& j, State& v) {
        j.at("players").get_to(v.players);
        j.at("objects").get_to(v.objects);
        j.at("terrain").get_to(v.terrain);
        j.at("now_turn").get_to(v.now_turn);
        j.at("map_size").get_to(v.map_size);
    }
    
    void to_json(json& j, const Object::Ptr& v) {
        j["posX"] = v->posX;
        j["posY"] = v->posY;
        j["type"] = "object";
        j["owner"] = v->owner;
        j["id"] = v->id;

        Bomb::Ptr bomb = dynamic_pointer_cast<Bomb>(v);
        if(bomb) {
            j["type"] = "bomb";
            j["ticks_left"] = bomb->ticks_left;
        }
    }
    void from_json(const json& it, Object::Ptr& v) {
        if(!it.at("posX").is_number_unsigned() ||
            !it.at("posY").is_number_unsigned())
            throw std::runtime_error("signed value provided (need unsigned for PlaceData)"); 

        if(it.at("type") == "bomb"){
            Bomb::Ptr bomb = std::make_shared<Bomb>(it.at("owner").get<std::string>(), it.at("id").get<ActorId>());
            it.at("ticks_left").get_to(bomb->ticks_left);
            v = bomb;
        }
        else{
            throw std::runtime_error("object type not implemented: " + it.at("type").get<std::string>());
        }
        it.at("posX").get_to(v->posX);
        it.at("posY").get_to(v->posY);
    }
    void to_json(json& j, const Event& v) {
        j["move_number"] = v.move_number;
        j["actor_id"] = v.actor_id;
        j["event_type"] = v.event_type;
        j["data"] = v.data;
    }
    void to_json(json& j, const VariantData& v) {
        if(std::holds_alternative<PlaceData>(v)){
            j["data"] = std::get<PlaceData>(v);
        }
        if(std::holds_alternative<DirectedPlaceData>(v)){
            j["data"] = std::get<DirectedPlaceData>(v);
        }
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