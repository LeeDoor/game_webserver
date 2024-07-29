#pragma once
#include <map>
#include <memory>
#include <optional>
#include "user.hpp"
#include "token.hpp"

namespace token_manager{
    class ITokenManager {
    public:
        using Ptr = std::shared_ptr<ITokenManager>; 
        virtual Token GenerateToken() = 0;
        virtual std::optional<um::Uuid> GetUuidByToken(const Token& token) = 0;
        virtual bool AddTokenToUuid(const Token& token, const um::Uuid& uuid) = 0;

        // for debug admin api functions only
        virtual std::map<Token, um::Uuid> GetValue() const = 0;
    };
}