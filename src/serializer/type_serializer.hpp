#include <nlohmann/json.hpp>
#include "registration_data.hpp"
#include "public_user.hpp"
#include "session.hpp"
using json = nlohmann::json;

namespace http_handler {
    void to_json(json& j, const RegistrationData& v);
    void from_json(const json& j, RegistrationData& v);
    
    void to_json(json& j, const PublicUser& v);
    void from_json(const json& j, PublicUser& v);
} 

namespace user_manager {
    void to_json(json& j, const User& v);
    void from_json(const json& j, User& v);
}

namespace game_manager{
    void to_json(json& j, const State& v);
    void from_json(const json& j, State& v);

    void to_json(json& j, const Player& v);
    void from_json(const json& j, Player& v);

    void to_json(json& j, const Obstacle& v);
    void from_json(const json& j, Obstacle& v);

    void to_json(json& j, const MapSize& v);
    void from_json(const json& j, MapSize& v);

    void to_json(json& j, const Session::WalkData& v);
    void from_json(const json& j, Session::WalkData& v);
    
    NLOHMANN_JSON_SERIALIZE_ENUM( Obstacle::Type, {
        {Obstacle::Type::Wall, "wall"},
    })

    NLOHMANN_JSON_SERIALIZE_ENUM( Session::MoveType, {
        {Session::MoveType::Walk, "walk"},
    })
}