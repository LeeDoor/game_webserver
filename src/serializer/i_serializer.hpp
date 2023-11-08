#pragma once
#include <string>
#include <map>
#include <optional>
#include <memory>
#include "registration_data.hpp"

namespace serializer{
    namespace dm = database_manager;
    class ISerializer {
    public:
        using Ptr = std::shared_ptr<ISerializer>;

        //serialize
        virtual std::string SerializeEmpty() = 0;
        virtual std::string SerializeError(std::string&& error_name, std::string&& desc) = 0;
        virtual std::string SerializeMap(std::map<std::string, std::string>&& map) = 0;

        //deserialize
        virtual std::optional<dm::RegistrationData> DeserializeUserData(const std::string& json) = 0;

    };
}