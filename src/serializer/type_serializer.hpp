#include <nlohmann/json.hpp>
#include "registration_data.hpp"
#include "public_user_data.hpp"
using json = nlohmann::json;

namespace http_handler {
    void to_json(json& j, const RegistrationData& rd);
    void from_json(const json& j, RegistrationData& rd);
    
    void to_json(json& j, const PublicUserData& rd);
    void from_json(const json& j, PublicUserData& rd);
} 

namespace database_manager {
    void to_json(json& j, const UserData& rd);
    void from_json(const json& j, UserData& rd);
}