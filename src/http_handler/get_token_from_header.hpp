#pragma once
#include "token.hpp"
#include <optional>
namespace http_handler{
    std::optional<tokenm::Token> GetTokenFromHeader(const std::string& header);
}
