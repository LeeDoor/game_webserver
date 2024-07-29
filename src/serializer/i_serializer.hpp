#pragma once
#include <string>
#include <map>
#include <vector>
#include <optional>
#include <memory>
#include "registration_data.hpp"
#include "public_user.hpp"
#include "token.hpp"
#include "../game_manager/session.hpp"

namespace serializer{
    namespace hh = http_handler;
    namespace tm = token_manager;
    namespace um = user_manager;
    class ISerializer {
    public:
        using Ptr = std::shared_ptr<ISerializer>;

        //serialize
        virtual std::string SerializeEmpty() = 0;
        virtual std::string SerializeError(const std::string& error_name, const std::string& desc) = 0;
        virtual std::string SerializeMap(std::map<std::string, std::string>&& map) = 0;
        
        virtual std::string Serialize(const hh::RegistrationData& rd) = 0;
        virtual std::string Serialize(const hh::PublicUser& pud) = 0;
        virtual std::string Serialize(const um::User& ud) = 0;
        virtual std::string Serialize(const std::map<tm::Token, um::Uuid>& ttu) = 0;
        virtual std::string Serialize(const std::vector<um::Uuid>& v) = 0;
        virtual std::string Serialize(const gm::State& state) = 0;
        virtual std::string Serialize(const gm::Session::WalkData& wd) = 0;

        //deserialize        
        virtual std::optional<std::map<std::string, std::string>> DeserializeMap(const std::string& json_str) = 0;

        virtual std::optional<hh::RegistrationData> DeserializeRegData(const std::string& json) = 0;
        virtual std::optional<hh::PublicUser> DeserializePublicUser(const std::string& json) = 0;
        virtual std::optional<um::User> DeserializeUser(const std::string& json) = 0;
        virtual std::optional<std::vector<um::Uuid>> DeserializeUuids(const std::string& json) = 0;
        virtual std::optional<gm::State> DeserializeSessionState(const std::string& json) = 0;

        virtual std::optional<gm::Session::MoveType> DefinePlayerMove(const std::string& json) = 0;
        virtual std::optional<gm::Session::WalkData> DeserializePlayerWalk(const std::string& json) = 0;
    };
}