#pragma once
#include "token_generator.hpp"
#include "i_token_manager.hpp"
#include <sw/redis++/redis++.h>

namespace token_manager{
    class TokenManagerRedis : public ITokenManager {
    public:
        using RedisPtr = std::shared_ptr<sw::redis::Redis>;

        TokenManagerRedis(std::string&& redis_data_name, RedisPtr rp);

        Token GenerateToken() override;
        std::optional<um::Uuid> GetUuidByToken(const Token& token) override;
        bool AddTokenToUuid(const Token& token, const um::Uuid& uuid) override;

        // for debug admin api functions only
        std::map<Token, um::Uuid> GetValue() const override;

    private:
        std::string redis_data_name_;

        RedisPtr redis_;
    };
}