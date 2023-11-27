#pragma once
#include <map>
#include <string>
#include <optional>
#include <boost/uuid/uuid.hpp>
#include <memory>
#include "token_generator.hpp"

namespace token_manager{
    class TokenToUuid {
    public:
        using Ptr = std::shared_ptr<TokenToUuid>; 

        Token GenerateToken();
        std::optional<boost::uuids::uuid> GetUuidByToken(const Token& token);
        void AddTokenToUuid(const Token& token, boost::uuids::uuid& uuid);

    private:
        std::map<Token, boost::uuids::uuid> map;
    };
}