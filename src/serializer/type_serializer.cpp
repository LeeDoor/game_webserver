#include "type_serializer.hpp"

namespace http_handler {
    void to_json(json& j, const RegistrationData& rd) {
        j = json{{"login", rd.login}, {"password", rd.password}};
    }

    void from_json(const json& j, RegistrationData& rd) {
        j.at("login").get_to(rd.login);
        j.at("password").get_to(rd.password);
    }
} 