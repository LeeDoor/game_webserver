#include "token_to_uuid.hpp"

namespace token_manager{
    Token TokenToUuid::GenerateToken() {
        TokenGenerator tg;
        Token t;
        do t = std::move(tg.GenerateToken());
        while (map.contains(t));

        return std::move(t);
    }
    std::optional<std::string> TokenToUuid::GetUuidByToken(const Token& token) {
        if(map.contains(token)){
            return map.at(token);
        }
        return std::nullopt;
    }
    void TokenToUuid::AddTokenToUuid(const Token& token, std::string& uuid) {
        if(map.contains(token)) map.erase(token);
        map[token] = uuid;
    }
    
}