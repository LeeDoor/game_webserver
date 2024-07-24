#pragma once
#include "i_serializer.hpp"

namespace serializer{
    class JSONSerializer : public ISerializer{
    public:
        std::string SerializeEmpty() override;
        std::string SerializeError(const std::string& error_name, const std::string& desc) override;
        std::string SerializeMap(std::map<std::string, std::string>&& map) override; 
        
        std::string Serialize(const hh::RegistrationData& rd) override;
        std::string Serialize(const dm::UserData& ud) override;
        std::string Serialize(const hh::PublicUserData& pud) override;
        std::string Serialize(const std::map<tm::Token,dm::Uuid>& ttu) override;
        std::string Serialize(const std::vector<dm::Uuid>& v) override;
        std::string Serialize(const gm::State& state) override;

        std::optional<hh::PublicUserData> DeserializePublicUserData(const std::string& json) override;
        std::optional<dm::UserData> DeserializeUserData(const std::string& json_str) override;
        std::optional<hh::RegistrationData> DeserializeRegData(const std::string& json) override;
        std::optional<std::map<std::string, std::string>> DeserializeMap(const std::string& json_str) override;
        std::optional<std::vector<dm::Uuid>> DeserializeUuids(const std::string& json) override;
        std::optional<gm::State> DeserializeSessionState(const std::string& json) override;

        std::optional<gm::Session::PlayerMoveType> DefinePlayerMove(const std::string& json) override;
        std::optional<gm::Session::WalkData> DeserializePlayerWalk(const std::string& json) override;
    };
}