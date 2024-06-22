#pragma once
#include <map>
#include <string>
#include <optional>
#include <memory>
#include "token_generator.hpp"

namespace token_manager{
    class TokenToUuid {
    public:
        using Ptr = std::shared_ptr<TokenToUuid>; 

        Token GenerateToken();
        std::optional<std::string> GetUuidByToken(const Token& token);
        void AddTokenToUuid(const Token& token, std::string& uuid);

        // for debug admin api functions only
        const std::map<Token, std::string>& GetValue() const;
    private:
        std::map<Token, std::string> map_;
    };
}