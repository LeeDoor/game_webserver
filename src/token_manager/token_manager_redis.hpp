#pragma once
#include "token_generator.hpp"
#include "i_token_manager.hpp"

namespace token_manager{
    class TokenManagerRedis : public ITokenManager {
    public:
        Token GenerateToken() override;
        std::optional<dm::Uuid> GetUuidByToken(const Token& token) override;
        void AddTokenToUuid(const Token& token, const dm::Uuid& uuid) override;

        // for debug admin api functions only
        std::map<Token, dm::Uuid> GetValue() const override;
    };
}