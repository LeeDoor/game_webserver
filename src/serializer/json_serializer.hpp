#pragma once
#include "i_serializer.hpp"

namespace serializer{
    class JSONSerializer : public ISerializer{
    public:
        std::string SerializeEmpty() override;
        std::string SerializeError(const std::string& error_name, const std::string& desc) override;
        std::string SerializeMap(std::map<std::string, std::string>&& map) override; 
        
        std::string SerializeRegData(const hh::RegistrationData& rd) override;
        std::string SerializeUserData(const dm::UserData& ud) override;
        std::string SerializePublicUserData(const hh::PublicUserData& pud) override;
        std::string SerializeTokenToUuid(const std::map<tm::Token,dm::Uuid>& ttu) override;
        std::string SerializeUuids(const std::vector<dm::Uuid>& v) override;

        std::optional<hh::PublicUserData> DeserializePublicUserData(const std::string& json) override;
        std::optional<dm::UserData> DeserializeUserData(const std::string& json_str) override;
        std::optional<hh::RegistrationData> DeserializeRegData(const std::string& json) override;
        std::optional<std::map<std::string, std::string>> DeserializeMap(const std::string& json_str) override;
        std::optional<std::vector<dm::Uuid>> DeserializeUuids(const std::string& json) override;
    };
}