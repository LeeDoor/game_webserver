#include "json_serializer.hpp"
#include "json_type_converter.hpp"
#include "type_serializer.hpp"
#define EMPTY_JSON "{}"
#include <iostream>

namespace serializer{
    std::string JSONSerializer::SerializeEmpty() {
        return EMPTY_JSON;
    }
    std::string JSONSerializer::SerializeError(std::string&& error_name, std::string&& desc) {
        nlohmann::json obj = nlohmann::json::object();
        obj["error_name"] = std::move(error_name);
        obj["description"] = std::move(desc);
        return obj.dump();
    }
    std::string JSONSerializer::SerializeMap(std::map<std::string, std::string>&& map) {
        nlohmann::json obj;
        for (const auto& pair : map) {
            obj[pair.first] = pair.second;
        }
        return obj.dump();
    } 
    std::string JSONSerializer::SerializeRegData(const hh::RegistrationData& rd) {
        nlohmann::json json = rd;
        return json.dump();
    }
    std::string JSONSerializer::SerializePublicUserData(const hh::PublicUserData& pud) {
        nlohmann::json json = pud;
        return json.dump();
    }
    std::string JSONSerializer::SerializeUserData(const dm::UserData& ud) {
        nlohmann::json json = ud;
        return json.dump();
    }
    std::string JSONSerializer::SerializeTokenToUuid(const std::map<tm::Token,dm::Uuid>& ttu) {
        if(ttu.empty()){
            return SerializeEmpty();
        }
        nlohmann::json obj;
        for (const auto& pair : ttu) {
            obj[pair.first] = pair.second;
        }
        return obj.dump();
    }

    std::optional<hh::PublicUserData> JSONSerializer::DeserializePublicUserData(const std::string& json_str) {
        hh::PublicUserData pud;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            pud = j.template get<hh::PublicUserData>();
        }
        catch(std::exception ex){
            std::cout << ex.what() << std::endl;
            return std::nullopt;
        }
        return pud;
    }
    std::optional<dm::UserData> JSONSerializer::DeserializeUserData(const std::string& json_str) {
        dm::UserData ud;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            ud = j.template get<dm::UserData>();
        }
        catch(std::exception ex){
            std::cout << ex.what() << std::endl;
            return std::nullopt;
        }
        return ud;
    }
    std::optional<hh::RegistrationData> JSONSerializer::DeserializeRegData(const std::string& json_str) {
        hh::RegistrationData rd;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            rd = j.template get<hh::RegistrationData>();
        }
        catch(std::exception ex){
            std::cout << ex.what() << std::endl;
            return std::nullopt;
        }
        return rd;
    }
    std::optional<std::map<std::string, std::string>> JSONSerializer::DeserializeMap(const std::string& json_str) {
        std::map<std::string, std::string> map;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            for(auto pair : j.items()){
                map[pair.key()] = pair.value();
            }
        }
        catch(std::exception ex){
            std::cout << ex.what() << std::endl;
            return std::nullopt;
        }
        return map;
    }
}
