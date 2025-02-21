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
        std::string static_path;
        std::string postgres_credentials;
        std::string postgres_address;
        std::string redis_credentials;
        std::string redis_address;
        std::string log_dir;
    };

    int StartServer(Args args);
    Args ParseParameters(int argc, char** argv);
};

}
