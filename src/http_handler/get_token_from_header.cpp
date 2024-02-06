#include "get_token_from_header.hpp"
namespace http_handler{
    std::optional<tokenm::Token> GetTokenFromHeader(const std::string& header){
        if(header.substr(0, 7) == "Bearer "){
            tokenm::Token token = header.substr(7);
            if(token.size() == 32)
                return token;
        }
        return std::nullopt;
    }
}