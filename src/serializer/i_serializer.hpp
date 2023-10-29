#pragma once
#include <string>
#include <map>
#include <optional>
#include "registration_data.hpp"

namespace serializer{
    class ISerializer {
    public:
        //serialize
        virtual std::string SerializeEmpty() = 0;
        virtual std::string SerializeError(std::string&& error_name, std::string&& desc) = 0;
        virtual std::string SerializeMap(std::map<std::string, std::string>&& map) = 0;

        //deserialize
        virtual std::optional<user_data::RegistrationData> DeserializeRegData(std::string&& json) = 0; 
    };
}