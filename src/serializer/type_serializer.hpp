#include <nlohmann/json.hpp>
#include "registration_data.hpp"
#include "public_user_data.hpp"
#include "session.hpp"
using json = nlohmann::json;

namespace http_handler {
    void to_json(json& j, const RegistrationData& v);
    void from_json(const json& j, RegistrationData& v);
    
    void to_json(json& j, const PublicUserData& v);
    void from_json(const json& j, PublicUserData& v);
} 

namespace database_manager {
    void to_json(json& j, const UserData& v);
    void from_json(const json& j, UserData& v);
}

namespace game_manager{
    void to_json(json& j, const State& v);
    void from_json(const json& j, State& v);

    void to_json(json& j, const Player& v);
    void from_json(const json& j, Player& v);

    void to_json(json& j, const Obstacle& v);
    void from_json(const json& j, Obstacle& v);

    void to_json(json& j, const Session::WalkData& v);
    void from_json(const json& j, Session::WalkData& v);
    
    NLOHMANN_JSON_SERIALIZE_ENUM( Obstacle::Type, {
        {Obstacle::Type::Wall, "wall"},
    })

    NLOHMANN_JSON_SERIALIZE_ENUM( Session::PlayerMoveType, {
        {Session::PlayerMoveType::Walk, "walk"},
    })
}