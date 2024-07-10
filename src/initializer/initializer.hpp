#pragma once
#include <string>
namespace initializer {

class Initializer {
public:
    int Init(int argc, char** argv);
private:
    struct Args {
        int test;
        int port;
        std::string db_login;
        std::string db_password;
        std::string static_path;
        std::string api_path;
        std::string postgres_credentials;
        std::string redis_credentials;
    };

    int StartServer(Args args);
    Args ParseParameters(int argc, char** argv);
};

}
