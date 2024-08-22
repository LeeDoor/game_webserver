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
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NewObject, actor_id);
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