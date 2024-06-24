#include "type_serializer.hpp"

namespace http_handler {
    void to_json(json& j, const RegistrationData& rd) {
        j = json{{"login", rd.login}, {"password", rd.password}};
    }
    void from_json(const json& j, RegistrationData& rd) {
        j.at("login").get_to(rd.login);
        j.at("password").get_to(rd.password);
    }

    void to_json(json& j, const PublicUserData& rd) {
        j = json{{"login", rd.login}, {"password", rd.password}};
    }
    void from_json(const json& j, PublicUserData& rd) {
        j.at("login").get_to(rd.login);
        j.at("password").get_to(rd.password);
    }
}
namespace database_manager {
    void to_json(json& j, const UserData& rd) {
        j = json{{"uuid", rd.uuid}, {"login", rd.login}, {"password", rd.password}};
    }
    void from_json(const json& j, UserData& rd) {
        j.at("uuid").get_to(rd.uuid);
        j.at("login").get_to(rd.login);
        j.at("password").get_to(rd.password);
    }
} 