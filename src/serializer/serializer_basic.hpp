#pragma once
#include <string>
#include <map>
#include <optional>

namespace serializer{
    std::string SerializeEmpty();
    std::string SerializeError(const std::string& error_name, const std::string& desc);
    std::string SerializeMap(std::map<std::string, std::string>&& map); 

    std::optional<std::map<std::string, std::string>> DeserializeMap(const std::string& json_str);
}