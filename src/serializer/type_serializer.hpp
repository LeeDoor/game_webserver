#include <nlohmann/json.hpp>
#include "registration_data.hpp"
#include "public_user.hpp"
#include "session_data.hpp"
#include "../game_manager/session.hpp"
#include "move_data.hpp"
using json = nlohmann::json;

namespace http_handler {
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RegistrationData, login, password);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PublicUser, login, password);
} 

namespace user_manager {
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(User, uuid, login, password);
}

namespace game_manager{
    void to_json(json& j, const State& v);
    void from_json(const json& j, State& v);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Position, x, y)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Player, login, position, actor_id)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Obstacle, actor_id, position, type)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapSize, width, height)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PosMoveData, position)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DirPosMoveData, position, direction)
    
    NLOHMANN_JSON_SERIALIZE_ENUM( Obstacle::Type, {
        {Obstacle::Type::Wall, "wall"},
    })

    NLOHMANN_JSON_SERIALIZE_ENUM( ::gm::Session::MoveType, {
        {Session::MoveType::Walk, "walk"},
        {Session::MoveType::Resign, "resign"},
        {Session::MoveType::PlaceBomb, "place_bomb"},
        {Session::MoveType::PlaceGun, "place_gun"},
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(Direction, {
        {Direction::Up, "up"},
        {Direction::Right, "right"},
        {Direction::Down, "down"},
        {Direction::Left, "left"},
    })
}

namespace session_manager{
    void to_json(json& j, const PublicSessionData& v);
    void from_json(const json& j, PublicSessionData& v);
}

namespace nlohmann {
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        static void to_json(json& j, const std::shared_ptr<T>& value) {
            if (value.get()) {
                j = *value;
            } else {
                j = nullptr;
            }
        }
        
        static void from_json(const json& j, std::shared_ptr<T>& value) {
            if (j.is_null()) {
                value = nullptr;
            } else {
                value = std::make_shared<T>(j.template get<T>());
            }
        }
    };
    template <>
    struct adl_serializer<std::shared_ptr<game_manager::Event>> {
        static void to_json(json& j, const std::shared_ptr<game_manager::Event>& v) {
            v->tojson(j);
        }
    };
    template <>
    struct adl_serializer<std::shared_ptr<game_manager::Object>> {
        static void to_json(json& j, const std::shared_ptr<game_manager::Object>& v) {
            v->tojson(j);
        }
        
        static void from_json(const json& it, std::shared_ptr<game_manager::Object>& v) {
            namespace gm = game_manager;

            gm::Object::OwnerType owner;
            gm::ActorId actor_id;
            gm::Position position;
            owner = it.at("owner").get<gm::Object::OwnerType>();
            actor_id = it.at("actor_id").get<gm::ActorId>();
            position = it.at("position").get<gm::Position>();
            if(it.at("type") == "bomb"){
                gm::Bomb::Ptr bomb = std::make_shared<gm::Bomb>(owner, actor_id);
                it.at("ticks_left").get_to(bomb->ticks_left);
                bomb->Place(position);
                v = bomb;
            }
            else if(it.at("type") == "gun"){
                gm::Gun::Ptr gun = std::make_shared<gm::Gun>(owner, actor_id);
                it.at("ticks_to_shot").get_to(gun->ticks_to_shot);
                it.at("shots_left").get_to(gun->shots_left);
                gun->Place(position, it.at("direction").get<gm::Direction>());
                v = gun;
            }
            else if(it.at("type") == "bullet"){
                gm::Bullet::Ptr bullet = std::make_shared<gm::Bullet>(owner, actor_id);
                bullet->Place(position, it.at("direction").get<gm::Direction>());
                v = bullet;
            }
            else{
                throw std::runtime_error("object type not implemented: " + it.at("type").get<std::string>());
            } 
        }
    };
}