#pragma once
#include <random>
#include "token.hpp"

namespace database_manager{
    class TokenGenerator {
    public:
        Token GenerateToken();
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
}
