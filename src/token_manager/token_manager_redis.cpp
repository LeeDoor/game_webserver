#include "token_manager_redis.hpp"
#include "token_generator.hpp"

namespace token_manager{
    TokenManagerRedis::TokenManagerRedis(std::string&& redis_data_name, RedisPtr rp):
        redis_(rp), redis_data_name_(redis_data_name){}

    Token TokenManagerRedis::GenerateToken() {
        TokenGenerator tg;
        Token t;
        do t = std::move(tg.GenerateToken());
        while (GetUuidByToken(t) != std::nullopt);

        return std::move(t);
    }

    std::optional<um::Uuid> TokenManagerRedis::GetUuidByToken(const Token& token) {
    	sw::redis::OptionalString os;
        os = redis_->hget(redis_data_name_, token);
        if (os)
            return *os;
        return std::nullopt;
    }

    bool TokenManagerRedis::AddTokenToUuid(const Token& token, const um::Uuid& uuid) {
        redis_->hset(redis_data_name_, token, uuid);
        auto uuid_opt = GetUuidByToken(token);
        return uuid_opt.has_value() && *uuid_opt == uuid; // hset returns 0 even if line edited (edited != added)
    }

    std::map<Token, um::Uuid> TokenManagerRedis::GetValue() const {
        std::map<Token, um::Uuid> res;
    	redis_->hgetall(redis_data_name_, std::inserter(res, res.begin()));
        return res;
    }
}