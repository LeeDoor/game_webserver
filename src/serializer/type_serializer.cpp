#include "type_serializer.hpp"

#include "spdlog/spdlog.h"
namespace http_handler {
    void to_json(json& j, const RegistrationData& v) {
        j = json{{"login", v.login}, {"password", v.password}};
    }
    void from_json(const json& j, RegistrationData& v) {
        j.at("login").get_to(v.login);
        j.at("password").get_to(v.password);
    }

    void to_json(json& j, const PublicUserData& v) {
        j = json{{"login", v.login}, {"password", v.password}};
    }
    void from_json(const json& j, PublicUserData& v) {
        j.at("login").get_to(v.login);
        j.at("password").get_to(v.password);
    }
}
namespace database_manager {
    void to_json(json& j, const UserData& v) {
        j = json{{"uuid", v.uuid}, {"login", v.login}, {"password", v.password}};
    }
    void from_json(const json& j, UserData& v) {
        j.at("uuid").get_to(v.uuid);
        j.at("login").get_to(v.login);
        j.at("password").get_to(v.password);
    }
} 
namespace game_manager{
    void to_json(json& j, const State& v) {
        j = json{{"players", v.players}, {"terrain", v.terrain}, {"now_turn", v.now_turn}};
    }
    void from_json(const json& j, State& v) {
        j.at("players").get_to(v.players);
        j.at("terrain").get_to(v.terrain);
        j.at("now_turn").get_to(v.now_turn);
    }

    void to_json(json& j, const Player& v) {
        j = json{{"login", v.login}, {"posX", v.posX}, {"posY", v.posY}};
    }
    void from_json(const json& j, Player& v) {
        j.at("login").get_to(v.login);
        j.at("posX").get_to(v.posX);
        j.at("posY").get_to(v.posY);
    }

    void to_json(json& j, const Obstacle& v) {
        j = json{{"posX", v.posX}, {"posY", v.posY}, {"type", v.type}};
    }
    void from_json(const json& j, Obstacle& v) {
        j.at("posX").get_to(v.posX);
        j.at("posY").get_to(v.posY);
        j.at("type").get_to(v.type);
    }

    void to_json(json& j, const Session::WalkData& v) {
        j = json{{"posX", v.posX}, {"posY", v.posY}};
    }
    void from_json(const json& j, Session::WalkData& v) {
        j.at("posX").get_to(v.posX);
        j.at("posY").get_to(v.posY);
    }
}