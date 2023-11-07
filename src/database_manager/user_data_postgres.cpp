#include "user_data_postgres.hpp"
#include <boost/uuid/uuid_generators.hpp>

namespace database_manager{    
    ids::uuid UserDataPostgres::GenerateUuid() {
        return ids::random_generator()();
    }
    bool UserDataPostgres::AddLine(UserData&& user_data) {
        if(std::find_if(user_data_.begin(), user_data_.end(), [&](UserData& ud){
            return ud.login == user_data.login;
        }) != user_data_.end()){
            return false;
        }
        user_data_.push_back(std::move(user_data));
        return true;
    }
    std::optional<UserData> UserDataPostgres::GetByUuid(ids::uuid&& uuid) {
        auto iter = std::find_if(user_data_.begin(), user_data_.end(), [&](UserData& ud){
            return ud.uuid == uuid;
        });
        if (iter == user_data_.end())
            return std::nullopt;
        return *iter;
    }
    std::optional<UserData> UserDataPostgres::GetByLoginPassword(std::string&& login, std::string&& password) {
        auto iter = std::find_if(user_data_.begin(), user_data_.end(), [&](UserData& ud){
            return ud.login == login && ud.password == password;
        });
        if (iter == user_data_.end())
            return std::nullopt;
        return *iter;
    }

}