#include "user_data_postgres.hpp"
#include "postgres_data_conversion.hpp"
#include <iostream>

namespace database_manager{    
    UserDataPostgres::UserDataPostgres() : pool_(CONNECTION_CAPACITY){}

    bool UserDataPostgres::AddLine(UserData& user_data) {
        try{
            ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
            pqxx::work trans(*cw);
            pqxx::row uuid_pqxx = trans.exec1("SELECT gen_random_uuid();");
            user_data.uuid = uuid_pqxx[0].as<std::string>();
            std::cout << user_data.login.size() << " " << user_data.password.size() << std::endl;
            trans.exec_params0("INSERT INTO users VALUES ($1, $2, $3);", user_data.uuid, user_data.login, user_data.password);
            trans.commit();
            return true;
        }
        catch(std::exception& ex){
            std::cout << ex.what() << std::endl;
        }
        return false;
    }
    std::optional<UserData> UserDataPostgres::GetByUuid(const std::string& uuid) {
        ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
        pqxx::read_transaction trans(*cw);

        try {
            pqxx::row res = 
                trans.exec_params1("SELECT * FROM users WHERE id=$1;", uuid);
            UserData ud = from_result(res);
            return ud;
        } 
        catch (const std::exception& ex) {
            std::cout << ex.what() << std::endl;
            return std::nullopt;
        }
        
    }
    std::optional<UserData> UserDataPostgres::GetByLoginPassword(const std::string& login, const std::string& password) {
        ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
        pqxx::read_transaction trans(*cw);

        try {
            pqxx::row res = 
                trans.exec_params1("SELECT * FROM users WHERE login=$1 AND password=$2;", login, password);
            UserData ud = from_result(res);
            return ud;
        } 
        catch (const std::exception& ex) {
            std::cout << ex.what() << std::endl;
            return std::nullopt;
        }
    }

}