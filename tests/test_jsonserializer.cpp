#include <catch2/catch_test_macros.hpp>
#include "json_serializer.hpp"
#include <nlohmann/json.hpp>

using namespace serializer;
using json = nlohmann::json;

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
void compare_errors(const Error& given, const Error& expected){
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
    JSONSerializer serializer;
    REQUIRE(serializer.SerializeEmpty() == "{}");
}

TEST_CASE("SerializeError", "[jsonserializer]") {
    JSONSerializer serializer;
    json j;
    Error given;

    REQUIRE_NOTHROW(j = json::parse(serializer.SerializeError("error name 1", "description 1")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    compare_errors(given, {"error name 1", "description 1"});

    REQUIRE_NOTHROW(j = json::parse(serializer.SerializeError("", "description 1")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    compare_errors(given, {"", "description 1"});

    REQUIRE_NOTHROW(j = json::parse(serializer.SerializeError("error name 1", "")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    compare_errors(given, {"error name 1", ""});

    REQUIRE_NOTHROW(j = json::parse(serializer.SerializeError("", "")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    compare_errors(given, {"", ""});
}

TEST_CASE("SerializeMap", "[jsonserializer]") {
    using StringMap = std::map<std::string, std::string>;
    
    JSONSerializer serializer;
    StringMap map;
    StringMap given;
    json j;

    map = {{"first", "second"}, {"third", "fourth"}};
    REQUIRE_NOTHROW(j = json::parse(serializer.SerializeMap(std::move(map))));
    REQUIRE_NOTHROW(given = j.template get<StringMap>());
    CHECK(map == given);

    map = {{"", ""}};
    REQUIRE_NOTHROW(j = json::parse(serializer.SerializeMap(std::move(map))));
    REQUIRE_NOTHROW(given = j.template get<StringMap>());
    CHECK(map == given);
}