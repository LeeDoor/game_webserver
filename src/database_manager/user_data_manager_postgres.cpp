#include "user_data_manager_postgres.hpp"
#include "postgres_data_conversion.hpp"
#include "spdlog/spdlog.h"

namespace database_manager{    
    UserDataManagerPostgres::UserDataManagerPostgres(bool is_test, std::string&& bd_credentials) 
    : pool_(CONNECTION_CAPACITY, is_test, std::move(bd_credentials)){
        if(is_test){
            try{
                ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
                pqxx::work trans(*cw);
                trans.exec("DELETE FROM users;");
                trans.commit();
            }
            catch(std::exception& ex){
                spdlog::error("test database clearance terminated with {}", ex.what());
            }
        }
    }

    bool UserDataManagerPostgres::AddLine(hh::RegistrationData& rd) {
        try{
            ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
            pqxx::work trans(*cw);
            pqxx::row uuid_pqxx = trans.exec1("SELECT gen_random_uuid();");
            trans.exec_params0("INSERT INTO users VALUES ($1, $2, $3);", uuid_pqxx[0].as<Uuid>(), rd.login, rd.password);
            trans.commit();
            return true;
        }
        catch(std::exception& ex){
            spdlog::error("database AddLine function terminated with {}", ex.what());
        }
        return false;
    }
    std::optional<UserData> UserDataManagerPostgres::GetByUuid(const Uuid& uuid) {
        ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
        pqxx::read_transaction trans(*cw);

        try {
            pqxx::row res = 
                trans.exec_params1("SELECT * FROM users WHERE id=$1;", uuid);
            UserData ud = from_result(res);
            return ud;
        } 
        catch (const std::exception& ex) {
            spdlog::error("database GetByUuid function terminated with {}", ex.what());
            return std::nullopt;
        }
        
    }
    std::optional<UserData> UserDataManagerPostgres::GetByLoginPassword(const Login& login, const Password& password) {
        ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
        pqxx::read_transaction trans(*cw);

        try {
            pqxx::row res = 
                trans.exec_params1("SELECT * FROM users WHERE login=$1 AND password=$2;", login, password);
            UserData ud = from_result(res);
            return ud;
        } 
        catch (const std::exception& ex) {  
            spdlog::error("database GetByLoginPassword function terminated with {}", ex.what());
            return std::nullopt;
        }
    }

}