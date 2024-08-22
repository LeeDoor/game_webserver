#include <nlohmann/json.hpp>
#include "registration_data.hpp"
#include "public_user.hpp"
#include "session.hpp"
#include "session_data.hpp"
#include "object.hpp"
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
    void to_json(json& j, const Object::Ptr& v);
    void from_json(const json& j, Object::Ptr& v);
    void to_json(json& j, const VariantEventData& v);
    void to_json(json& j, const Event& v);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Player, login, posX, posY, id);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Obstacle, posX, posY, type);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapSize, width, height);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlaceData, posX, posY);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DirectedPlaceData, posX, posY, dir);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Objectdata, actor_id);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BombData, place, new_object, ticks_left);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WalkData, place);
    
    NLOHMANN_JSON_SERIALIZE_ENUM( Obstacle::Type, {
        {Obstacle::Type::Wall, "wall"},
    })

    NLOHMANN_JSON_SERIALIZE_ENUM( Session::MoveType, {
        {Session::MoveType::Walk, "walk"},
        {Session::MoveType::Resign, "resign"},
        {Session::MoveType::PlaceBomb, "place_bomb"},
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
    struct adl_serializer<std::shared_ptr<game_manager::Object>> {
        static void to_json(json& j, const std::shared_ptr<game_manager::Object>& v) {
            v->tojson(j);
        }
        
        static void from_json(const json& it, std::shared_ptr<game_manager::Object>& v) {
            namespace gm = game_manager;

            gm::Object::OwnerType owner;
            gm::ActorId actor_id;
            owner = it.at("owner").get<gm::Object::OwnerType>();
            actor_id = it.at("actor_id").get<gm::ActorId>();
            if(it.at("type") == "bomb"){
                gm::Bomb::Ptr bomb = std::make_shared<gm::Bomb>(owner, actor_id);
                it.at("ticks_left").get_to(bomb->ticks_left);
                bomb->Place(it.at("posX").get<gm::Dimention>(), it.at("posY").get<gm::Dimention>());
                v = bomb;
                return;
            }
            if(it.at("type" == "gun")){
                gm::Gun::Ptr gun = std::make_shared<gm::Gun>(owner, actor_id);
                it.at("ticks_to_shot").get_to(gun->ticks_to_shot);
                it.at("shots_left").get_to(gun->shots_left);
                gun->Place(it.at("posX").get<gm::Dimention>(), it.at("posY").get<gm::Dimention>(), it.at("dir").get<gm::Direction>()); 
                v = gun;
                return;
            }
            if(it.at("type" == "bullet")){
                gm::Bullet::Ptr bullet = std::make_shared<gm::Bullet>(owner, actor_id);
                bullet->Place(it.at("posX").get<gm::Dimention>(), it.at("posY").get<gm::Dimention>(), it.at("dir").get<gm::Direction>());
                v = bullet;
                return;
            }
            else{
                throw std::runtime_error("object type not implemented: " + it.at("type").get<std::string>());
            } 
        }
    };
}