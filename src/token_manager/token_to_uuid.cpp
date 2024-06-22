#include "token_to_uuid.hpp"

namespace token_manager{
    Token TokenToUuid::GenerateToken() {
        TokenGenerator tg;
        Token t;
        do t = std::move(tg.GenerateToken());
        while (map_.contains(t));

        return std::move(t);
    }
    std::optional<std::string> TokenToUuid::GetUuidByToken(const Token& token) {
        if(map_.contains(token)){
            return map_.at(token);
        }
        return std::nullopt;
    }
    void TokenToUuid::AddTokenToUuid(const Token& token, std::string& uuid) {
        if(map_.contains(token)) map_.erase(token);
        map_[token] = uuid;
    }
    
    const std::map<Token, std::string>& TokenToUuid::GetValue() const {
        return map_;
    }           
}