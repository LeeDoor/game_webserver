#include "initializer/initializer.hpp"

int main(int argc, char** argv) {
    initializer::Initializer initializer;
    return initializer.init(argc, argv);
}