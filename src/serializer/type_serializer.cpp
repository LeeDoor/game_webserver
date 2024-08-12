#include "type_serializer.hpp"
#include "spdlog/spdlog.h"
#include "bomb.hpp"

namespace http_handler {
    void to_json(json& j, const RegistrationData& v) {
        j = json{{"login", v.login}, {"password", v.password}};
    }
    void from_json(const json& j, RegistrationData& v) {
        j.at("login").get_to(v.login);
        j.at("password").get_to(v.password);
    }

    void to_json(json& j, const PublicUser& v) {
        j = json{{"login", v.login}, {"password", v.password}};
    }
    void from_json(const json& j, PublicUser& v) {
        j.at("login").get_to(v.login);
        j.at("password").get_to(v.password);
    }
}
namespace user_manager {
    void to_json(json& j, const User& v) {
        j = json{{"uuid", v.uuid}, {"login", v.login}, {"password", v.password}};
    }
    void from_json(const json& j, User& v) {
        j.at("uuid").get_to(v.uuid);
        j.at("login").get_to(v.login);
        j.at("password").get_to(v.password);
    }
} 
namespace game_manager{
    void to_json(json& j, const State& v) {
        j = json{
            {"state", "playing"}, 
            {"players", v.players}, 
            {"objects", v.objects}, 
            {"terrain", v.terrain}, 
            {"now_turn", v.now_turn},
            {"map_size", v.map_size},
            {"events", v.events},
        };
    }
    void from_json(const json& j, State& v) {
        j.at("players").get_to(v.players);
        j.at("objects").get_to(v.objects);
        j.at("terrain").get_to(v.terrain);
        j.at("now_turn").get_to(v.now_turn);
        j.at("map_size").get_to(v.map_size);
        j.at("events").get_to(v.events);
    }

    void to_json(json& j, const Player& v) {
        j = json{{"login", v.login}, {"posX", v.posX}, {"posY", v.posY}, {"id", v.id}};
    }
    void from_json(const json& j, Player& v) {
        j.at("login").get_to(v.login);
        j.at("posX").get_to(v.posX);
        j.at("posY").get_to(v.posY);
        j.at("id").get_to(v.id);
    }

    void to_json(json& j, const Obstacle& v) {
        j = json{{"posX", v.posX}, {"posY", v.posY}, {"type", v.type}};
    }
    void from_json(const json& j, Obstacle& v) {
        j.at("posX").get_to(v.posX);
        j.at("posY").get_to(v.posY);
        j.at("type").get_to(v.type);
    }

    void to_json(json& j, const MapSize& v) {
        j = json{{"width", v.width}, {"height", v.height}};
    }
    void from_json(const json& j, MapSize& v) {
        if(!j.at("width").is_number_unsigned() ||
           !j.at("height").is_number_unsigned())
            throw std::runtime_error("signed value provided (need unsigned for Session::PlaceData)");
        j.at("width").get_to(v.width);
        j.at("height").get_to(v.height);
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
            throw std::runtime_error("signed value provided (need unsigned for Session::PlaceData)");

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
        j = json{{"event_type", v.event_type}, {"id", v.id}};
    }
    void from_json(const json& j, Event& v) {
        j.at("event_type").get_to(v.event_type);
        j.at("id").get_to(v.id);
    }

    void to_json(json& j, const Session::PlaceData& v) {
        j = json{{"posX", v.posX}, {"posY", v.posY}};
    }
    void from_json(const json& j, Session::PlaceData& v) {
        if(!j.at("posX").is_number_unsigned() ||
           !j.at("posY").is_number_unsigned())
            throw std::runtime_error("signed value provided (need unsigned for Session::PlaceData)");
        j.at("posX").get_to(v.posX);
        j.at("posY").get_to(v.posY);
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