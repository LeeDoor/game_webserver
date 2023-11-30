#pragma once
#include "i_serializer.hpp"

namespace serializer{
    class JSONSerializer : public ISerializer{
    public:
        std::string SerializeEmpty() override;
        std::string SerializeError(std::string&& error_name, std::string&& desc) override;
        std::string SerializeMap(std::map<std::string, std::string>&& map) override; 
        std::string SerializeRegData(const hh::RegistrationData& rd) override;
        std::string SerializePublicUserData(const hh::PublicUserData& pud) override;

        std::optional<hh::PublicUserData> DeserializePublicUserData(const std::string& json) override;
        std::optional<hh::RegistrationData> DeserializeRegData(const std::string& json) override;
        std::optional<std::map<std::string, std::string>> DeserializeMap(const std::string& json_str) override;
    };
}