#include "token_to_uuid.hpp"

namespace database_manager{
    Token TokenToUuid::GenerateToken() {
        TokenGenerator tg;
        Token t;
        do t = std::move(tg.GenerateToken());
        while (map.contains(t));

        return std::move(t);
    }
    std::optional<boost::uuids::uuid> TokenToUuid::GetUuidByToken(const Token& token) {
        if(map.contains(token)){
            return map.at(token);
        }
        return std::nullopt;
    }
    void TokenToUuid::AddTokenToUuid(const Token& token, boost::uuids::uuid& uuid) {
        if(map.contains(token)) map.erase(token);
        map[token] = uuid;
    }
    
}