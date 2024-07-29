#pragma once
#include <random>
#include <iomanip>
#include "user.hpp"

class NickGenerator {
public:
    um::Login GenerateNick(){
        std::stringstream ss;
        uint64_t key1 = generator1_();
        ss << std::hex << std::setw(10) << std::setfill('0') << key1;
        return ss.str();
    }
private:
    std::random_device random_device_;

    std::mt19937_64 generator1_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
    std::mt19937_64 generator2_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
};

