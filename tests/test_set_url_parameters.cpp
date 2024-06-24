#include <catch2/catch_test_macros.hpp>
#include "api_functions.hpp"

TEST_CASE("SetUrlParameters tests", "[SetUrlParameters][function]") {
	std::string url, result;
	std::map<std::string, std::string> params;
	SECTION ("filled url"){
		url = "/api/get/my/socks/back";
		params = {{"", "value1"},{"key2",""}};
		result = SetUrlParameters(url, params);
		CHECK(result == "/api/get/my/socks/back?=value1&key2=");

		params = {{"", ""}};
		result = SetUrlParameters(url, params);
		CHECK(result == "/api/get/my/socks/back?=");

		params = {{"key1", "value1"},{"key2","value2"}};
		result = SetUrlParameters(url, params);
		CHECK(result == "/api/get/my/socks/back?key1=value1&key2=value2");

		params = {{"key2","value2"}};
		result = SetUrlParameters(url, params);
		CHECK(result == "/api/get/my/socks/back?key2=value2");

		params = {};
		result = SetUrlParameters(url, params);
		CHECK(result == "/api/get/my/socks/back");
	}
	SECTION("empty url") {
		url = "";
		params = {{"", "value1"},{"key2",""}};
		result = SetUrlParameters(url, params);
		CHECK(result == "?=value1&key2=");

		params = {{"", ""}};
		result = SetUrlParameters(url, params);
		CHECK(result == "?=");

		params = {{"key1", "value1"},{"key2","value2"}};
		result = SetUrlParameters(url, params);
		CHECK(result == "?key1=value1&key2=value2");

		params = {{"key2","value2"}};
		result = SetUrlParameters(url, params);
		CHECK(result == "?key2=value2");

		params = {};
		result = SetUrlParameters(url, params);
		CHECK(result == "");
	}
}