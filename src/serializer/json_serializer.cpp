#include "json_serializer.hpp"
#define EMPTY_JSON "{}"

namespace serializer{
    std::string JSONSerializer::SerializeEmpty() {
        return EMPTY_JSON;
    }
    std::string JSONSerializer::SerializeError(std::string&& error_name, std::string&& desc) {

    }
    std::string JSONSerializer::SerializeMap(std::map<std::string, std::string>&& map) {

    }
}