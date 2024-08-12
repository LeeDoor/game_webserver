#include <catch2/catch_test_macros.hpp>
#include "serializer_basic.hpp"
#include "serializer_game.hpp"
#include "serializer_user.hpp"
#include "serializer_http.hpp"
#include "serializer_session.hpp"
#include "registration_data.hpp"
#include "public_user.hpp"
#include "token.hpp"
#include "type_serializer.hpp"
#include "bomb.hpp"
#include <nlohmann/json.hpp>


using json = nlohmann::json;
namespace hh = http_handler;

struct Error{
    std::string error_name;
    std::string description;
};
void to_json(json& j, const Error& p) {
    j = json{{"error_name", p.error_name}, {"description", p.description}};
}
void from_json(const json& j, Error& p) {
    j.at("error_name").get_to(p.error_name);
    j.at("description").get_to(p.description);
}
void CompareErrors(const Error& given, const Error& expected){
    if(given.error_name != expected.error_name){
        FAIL_CHECK("error_name is wrong. given: " << given.error_name
                         << " expected: " << expected.error_name);
    }
    if(given.description != expected.description){
        FAIL_CHECK("description is wrong. given: " << given.description
                         << " expected: " << expected.description);
    }
}

TEST_CASE("SerializeEmpty", "[jsonserializer]"){
    REQUIRE(serializer::SerializeEmpty() == "{}");
}

TEST_CASE("SerializeError", "[jsonserializer]") {
    json j;
    Error given;

    REQUIRE_NOTHROW(j = json::parse(serializer::SerializeError("error name 1", "description 1")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    CompareErrors(given, {"error name 1", "description 1"});

    REQUIRE_NOTHROW(j = json::parse(serializer::SerializeError("", "description 1")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    CompareErrors(given, {"", "description 1"});

    REQUIRE_NOTHROW(j = json::parse(serializer::SerializeError("error name 1", "")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    CompareErrors(given, {"error name 1", ""});

    REQUIRE_NOTHROW(j = json::parse(serializer::SerializeError("", "")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    CompareErrors(given, {"", ""});
}

TEST_CASE("SerializeMap & DeserializeMap", "[jsonserializer]") {
    using StringMap = std::map<std::string, std::string>;
    
    StringMap map;
    StringMap given;
    json j;

    SECTION("SerializeMap"){
        map = {{"first", "second"}, {"third", "fourth"}};
        REQUIRE_NOTHROW(j = json::parse(serializer::SerializeMap(std::move(map))));
        REQUIRE_NOTHROW(given = j.template get<StringMap>());
        CHECK(map == given);

        map = {{"", ""}};
        REQUIRE_NOTHROW(j = json::parse(serializer::SerializeMap(std::move(map))));
        REQUIRE_NOTHROW(given = j.template get<StringMap>());
        CHECK(map == given);
    }
    SECTION("DeserializeMap"){
        map = {{"biber", "dolik"}, {"dodster", "pizza"}};
        for(auto pair : map){
            j[pair.first] = pair.second;
        }
        auto is_given = serializer::DeserializeMap(j.dump());
        REQUIRE(is_given);
        given = *is_given;
        for(auto pair : given){
            REQUIRE(map.contains(pair.first));
            CHECK(pair.second == map.at(pair.first));
        }
    }
}

TEST_CASE("Serialize & DeserializeRegData", "[jsonserializer]") {
    SECTION ("Serialize") {
        std::string rd_given = serializer::Serialize(hh::RegistrationData{"login1", "password1"});
        CHECK(rd_given == "{\"login\":\"login1\",\"password\":\"password1\"}");
        rd_given = serializer::Serialize(hh::RegistrationData{"login174387458578348735745687r576845785467956985957895678956789567", "passwor12344444444234234444444444444444444444444444444444444442342342d1"});
        CHECK(rd_given == "{\"login\":\"login174387458578348735745687r576845785467956985957895678956789567\",\"password\":\"passwor12344444444234234444444444444444444444444444444444444442342342d1\"}");
        rd_given = serializer::Serialize(hh::RegistrationData{"", ""});
        CHECK(rd_given == "{\"login\":\"\",\"password\":\"\"}");
        rd_given = serializer::Serialize(hh::RegistrationData{"123qwe123", ""});
        CHECK(rd_given == "{\"login\":\"123qwe123\",\"password\":\"\"}");
        rd_given = serializer::Serialize(hh::RegistrationData{"", "123qwe123"});
        CHECK(rd_given == "{\"login\":\"\",\"password\":\"123qwe123\"}");
        rd_given = serializer::Serialize(hh::RegistrationData{"login@@@``~~~==/.<>@#$!%^", "password@@@``~~~==/.<>@#$!%^"});
        CHECK(rd_given == "{\"login\":\"login@@@``~~~==/.<>@#$!%^\",\"password\":\"password@@@``~~~==/.<>@#$!%^\"}");
    }
    SECTION ("DeserializeRegData") {
        hh::RegistrationData rd;
        rd = {"login1", "password1"};
        std::optional<hh::RegistrationData> rd_given;
        rd_given = serializer::DeserializeRegData("{\"login\":\"login1\",\"password\":\"password1\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {"login174387458578348735745687r576845785467956985957895678956789567", "passwor12344444444234234444444444444444444444444444444444444442342342d1"};
        rd_given = serializer::DeserializeRegData("{\"login\":\"login174387458578348735745687r576845785467956985957895678956789567\",\"password\":\"passwor12344444444234234444444444444444444444444444444444444442342342d1\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {"", ""};
        rd_given = serializer::DeserializeRegData("{\"login\":\"\",\"password\":\"\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {"123123123", ""};
        rd_given = serializer::DeserializeRegData("{\"login\":\"123123123\",\"password\":\"\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {"login@@@``~~~==/.<>@#$!%^", "password@@@``~~~==/.<>@#$!%^"};
        rd_given = serializer::DeserializeRegData("{\"login\":\"login@@@``~~~==/.<>@#$!%^\",\"password\":\"password@@@``~~~==/.<>@#$!%^\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);
    }
}

TEST_CASE("Serialize & DeserializePublicUser", "[jsonserializer]") {
    SECTION ("Serialize") {
        std::string rd_given = serializer::Serialize(hh::PublicUser{{"uuid", "login1", "password1"}});
        CHECK(rd_given == "{\"login\":\"login1\",\"password\":\"password1\"}");
        rd_given = serializer::Serialize(hh::PublicUser{{"uuid", "login174387458578348735745687r576845785467956985957895678956789567", "passwor12344444444234234444444444444444444444444444444444444442342342d1"}});
        CHECK(rd_given == "{\"login\":\"login174387458578348735745687r576845785467956985957895678956789567\",\"password\":\"passwor12344444444234234444444444444444444444444444444444444442342342d1\"}");
        rd_given = serializer::Serialize(hh::PublicUser{{"uuid", "", ""}});
        CHECK(rd_given == "{\"login\":\"\",\"password\":\"\"}");
        rd_given = serializer::Serialize(hh::PublicUser{{"uuid", "123qwe123", ""}});
        CHECK(rd_given == "{\"login\":\"123qwe123\",\"password\":\"\"}");
        rd_given = serializer::Serialize(hh::PublicUser{{"uuid", "", "123qwe123"}});
        CHECK(rd_given == "{\"login\":\"\",\"password\":\"123qwe123\"}");
        rd_given = serializer::Serialize(hh::PublicUser{{"uuid", "login@@@``~~~==/.<>@#$!%^", "password@@@``~~~==/.<>@#$!%^"}});
        CHECK(rd_given == "{\"login\":\"login@@@``~~~==/.<>@#$!%^\",\"password\":\"password@@@``~~~==/.<>@#$!%^\"}");
    }
    SECTION ("DeserializePublicUser") {
        hh::PublicUser rd;
        rd = {{"uuid", "login1", "password1"}};
        std::optional<hh::PublicUser> rd_given;
        rd_given = serializer::DeserializePublicUser("{\"login\":\"login1\",\"password\":\"password1\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {{"uuid", "login174387458578348735745687r576845785467956985957895678956789567", "passwor12344444444234234444444444444444444444444444444444444442342342d1"}};
        rd_given = serializer::DeserializePublicUser("{\"login\":\"login174387458578348735745687r576845785467956985957895678956789567\",\"password\":\"passwor12344444444234234444444444444444444444444444444444444442342342d1\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {{"uuid", "", ""}};
        rd_given = serializer::DeserializePublicUser("{\"login\":\"\",\"password\":\"\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {{"uuid", "123123123", ""}};
        rd_given = serializer::DeserializePublicUser("{\"login\":\"123123123\",\"password\":\"\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {{"uuid", "login@@@``~~~==/.<>@#$!%^", "password@@@``~~~==/.<>@#$!%^"}};
        rd_given = serializer::DeserializePublicUser("{\"login\":\"login@@@``~~~==/.<>@#$!%^\",\"password\":\"password@@@``~~~==/.<>@#$!%^\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);
    }
}

TEST_CASE("Serialize token map", "[jsonserializer]"){
    using StringMap = std::map<token_manager::Token, um::Uuid>;
    
    StringMap map;
    StringMap given;
    json j;

    SECTION("Serialize"){
        map = {{"first", "second"}, {"third", "fourth"}};
        REQUIRE_NOTHROW(j = json::parse(serializer::SerializeMap(std::move(map))));
        REQUIRE_NOTHROW(given = j.template get<StringMap>());
        CHECK(map == given);

        map = {{"", ""}};
        REQUIRE_NOTHROW(j = json::parse(serializer::SerializeMap(std::move(map))));
        REQUIRE_NOTHROW(given = j.template get<StringMap>());
        CHECK(map == given);
    }
}

TEST_CASE("Serialize & DeserializeSessionState", "[jsonserializer]"){
    json j;
    gm::State given;
    gm::State example;

    SECTION("Serialize"){
        example.now_turn = "login1";
        example.players = {
            {
                6171467,
                "login1",
                5,
                5      
            },
            {
                315156,
                "login2",
                1,
                1
            }
        };
        example.terrain = {
            {2,2,gm::Obstacle::Type::Wall},
            {3,3,gm::Obstacle::Type::Wall}
        };
        example.map_size = {15,15};
        auto bomb = std::make_shared<game_manager::Bomb>("owner", 1);
        bomb->Place(5,5);
        example.objects = {bomb};
        std::string given_str;
        REQUIRE_NOTHROW(given_str = serializer::Serialize(std::move(example)));
        INFO(given_str);
        REQUIRE_NOTHROW(j = json::parse(given_str));
        REQUIRE_NOTHROW(given = j.template get<gm::State>());
        REQUIRE(example == given);

        example.map_size = {0,0};
        example.now_turn = "";
        example.players = {
            {
                1112,
                "",
                0,
                0      
            },
            {
                315156,
                "",
                0,
                0
            }
        };
        example.terrain = {
            {0,0,gm::Obstacle::Type::Wall},
            {0,0,gm::Obstacle::Type::Wall}
        };
        REQUIRE_NOTHROW(given_str = serializer::Serialize(example));
        REQUIRE_NOTHROW(j = json::parse(given_str));
        REQUIRE_NOTHROW(given = j.template get<gm::State>());
        REQUIRE(example == given);
    }
    SECTION("Deserialize"){
        std::string given_str = "{\"events\":[{\"id\":2,\"event_type\":\"NASRALLL\"}], \"map_size\":{\"width\":15,\"height\":15},\"objects\":[{\"type\":\"bomb\", \"posX\":1,\"posY\":1,\"ticks_left\": 5, \"owner\":\"NIGGER\", \"id\":1}],\"players\":[{\"login\":\"login number one\",\"posX\":1,\"posY\":2, \"id\":315156},{\"login\":\"login number twoo\",\"posX\":5,\"posY\":6, \"id\":315156}],\"terrain\":[{\"posX\":3,\"posY\":4,\"type\":\"wall\"},{\"posX\":2,\"posY\":1,\"type\":\"wall\"},{\"posX\":89,\"posY\":12222555,\"type\":\"wall\"}],\"now_turn\":\"login number one\"}";
        auto opt = serializer::DeserializeSessionState(given_str);
        REQUIRE(opt.has_value());
        REQUIRE_NOTHROW(given = *opt);
        example.now_turn = "login number one";
        example.players = {
            {
                315156,
                "login number one",
                1,
                2      
            },
            {
                315156,
                "login number twoo",
                5,
                6
            }
        };
        example.terrain = {
            {3,4,gm::Obstacle::Type::Wall},
            {2,1,gm::Obstacle::Type::Wall},
            {89,12222555,gm::Obstacle::Type::Wall}
        };
        example.map_size = {15,15};
        auto bomb = std::make_shared<game_manager::Bomb>("NIGGER", 1);
        bomb->ticks_left = 5;
        bomb->Place(1,1);
        example.objects = {bomb};
        example.events = {{2, "NASRALLL"}};
        j = given;
        INFO(j.dump());
        REQUIRE(given == example);

    }
}   