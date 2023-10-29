#include "json_serializer.hpp"
#include "json_type_converter.hpp"
#define EMPTY_JSON "{}"

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

    std::optional<user_data::RegistrationData> JSONSerializer::DeserializeRegData(std::string&& json_str) {
        user_data::RegistrationData rd;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            rd = j.template get<user_data::RegistrationData>();
        }
        catch(...){
            return std::nullopt;
        }
        return rd;
    }
}
