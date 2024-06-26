#pragma once
#include <string>
#include <map>
#include <vector>
#include <optional>
#include <memory>
#include "registration_data.hpp"
#include "public_user_data.hpp"
#include "token.hpp"

namespace serializer{
    namespace hh = http_handler;
    namespace tm = token_manager;
    namespace dm = database_manager;
    class ISerializer {
    public:
        using Ptr = std::shared_ptr<ISerializer>;

        //serialize
        virtual std::string SerializeEmpty() = 0;
        virtual std::string SerializeError(std::string&& error_name, std::string&& desc) = 0;
        virtual std::string SerializeMap(std::map<std::string, std::string>&& map) = 0;
        
        virtual std::string SerializeRegData(const hh::RegistrationData& rd) = 0;
        virtual std::string SerializePublicUserData(const hh::PublicUserData& pud) = 0;
        virtual std::string SerializeUserData(const dm::UserData& ud) = 0;
        virtual std::string SerializeTokenToUuid(const std::map<tm::Token, dm::Uuid>& ttu) = 0;
        virtual std::string SerializeUuids(const std::vector<dm::Uuid>& v) = 0;

        //deserialize        
        virtual std::optional<std::map<std::string, std::string>> DeserializeMap(const std::string& json_str) = 0;

        virtual std::optional<hh::RegistrationData> DeserializeRegData(const std::string& json) = 0;
        virtual std::optional<hh::PublicUserData> DeserializePublicUserData(const std::string& json) = 0;
        virtual std::optional<dm::UserData> DeserializeUserData(const std::string& json) = 0;
        virtual std::optional<std::vector<dm::Uuid>> DeserializeUuids(const std::string& json) = 0;
    };
}