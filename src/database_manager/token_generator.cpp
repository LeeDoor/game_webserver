#include "token_generator.hpp"
#include <sstream>
#include <iomanip>

namespace database_manager{
    Token TokenGenerator::GenerateToken(){
        std::stringstream ss;
        uint64_t key1 = generator1_();
        uint64_t key2 = generator2_();
        ss << std::hex << std::setw(16) << std::setfill('0') << key1;
        ss << std::hex << std::setw(16) << std::setfill('0') << key2;
        return Token{ss.str()};
    }
}