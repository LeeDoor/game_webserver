#pragma once
#include <string>
#include <map>
#include <optional>

namespace serializer{
    class ISerializer {
    public:
        //serialize
        virtual std::string SerializeEmpty() = 0;
        virtual std::string SerializeError(std::string&& error_name, std::string&& desc) = 0;
        virtual std::string SerializeMap(std::map<std::string, std::string>&& map) = 0;
    };
}