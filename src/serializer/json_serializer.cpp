#include "json_serializer.hpp"
#include <boost/json.hpp>
#define EMPTY_JSON "{}"

namespace serializer{
    namespace json = boost::json;

    std::string JSONSerializer::SerializeEmpty() {
        return EMPTY_JSON;
    }
    std::string JSONSerializer::SerializeError(std::string&& error_name, std::string&& desc) {
        json::object obj;
        obj["error_name"] = std::move(error_name);
        obj["description"] = std::move(desc);
        return json::serialize(obj);
    }
    std::string JSONSerializer::SerializeMap(std::map<std::string, std::string>&& map) {
        json::object obj;
        for (const auto& pair : map) {
            obj[pair.first] = pair.second;
        }
        return json::serialize(obj);
    }
}