#include "token_manager_redis.hpp"
#include "token_generator.hpp"

namespace token_manager{
    Token TokenManagerRedis::GenerateToken() {
        TokenGenerator tg;
        Token t;
        do t = std::move(tg.GenerateToken());
        while (GetUuidByToken(t) != std::nullopt);

        return std::move(t);
    }

    std::optional<dm::Uuid> TokenManagerRedis::GetUuidByToken(const Token& token) {
    	return std::nullopt;
    }

    void TokenManagerRedis::AddTokenToUuid(const Token& token, const dm::Uuid& uuid) {
    	return ;
    }

    std::map<Token, dm::Uuid> TokenManagerRedis::GetValue() const {
    	return {};
    }
}