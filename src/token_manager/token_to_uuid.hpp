#pragma once
#include <map>
#include <string>
#include <optional>
#include <memory>
#include "token_generator.hpp"
#include "user_data.hpp"

namespace token_manager{
    class TokenToUuid {
    public:
        using Ptr = std::shared_ptr<TokenToUuid>; 

        Token GenerateToken();
        std::optional<dm::Uuid> GetUuidByToken(const Token& token);
        void AddTokenToUuid(const Token& token, const dm::Uuid& uuid);

        // for debug admin api functions only
        const std::map<Token, dm::Uuid>& GetValue() const;
    private:
        std::map<Token, dm::Uuid> map_;
    };
}