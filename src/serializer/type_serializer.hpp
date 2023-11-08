#include <nlohmann/json.hpp>
#include "registration_data.hpp"
using json = nlohmann::json;

namespace database_manager {
    void to_json(json& j, const RegistrationData& rd);
    void from_json(const json& j, RegistrationData& rd);
} 