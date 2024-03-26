#pragma once
namespace initializer {

class Initializer {
public:
    int Init(int argc, char** argv);
private:
    struct Args {
        bool test;
    };

    int StartServer(Args args);
    Args ParseParameters(int argc, char** argv);
};

}
