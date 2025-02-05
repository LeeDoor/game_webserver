#pragma once

// Standard library includes
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <functional>

// Boost includes
#include <boost/asio.hpp>
#include <boost/beast.hpp>

// Testing framework include (Catch2)
#include <catch2/catch_test_macros.hpp>

// Project includes
#include "../src/http_handler/http_types.hpp"

// Create convenient namespace aliases
namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace hh    = http_handler;
using tcp = net::ip::tcp;

/// Template alias for std::optional.
template<typename T>
using opt = std::optional<T>;

/// Alias for a vector of strings.
using vecstr = std::vector<std::string>;

/// Shared pointer alias for StringResponse.
using ResponsePtr = std::shared_ptr<StringResponse>;

/// Connects a TCP socket using the provided io_context. 
/// This function may throw boost::system::system_error on failure.
void ConnectSocket(net::io_context& ioc, tcp::socket& socket);

/// Sends an HTTP request and returns the server response as a StringResponse.
/// @param is_head Specifies whether the request is a HEAD request.
/// @param request The HTTP request object. (May be modified during processing.)
/// @param socket A connected TCP socket.
/// @return A StringResponse containing the server's response.
StringResponse GetResponseToRequest(bool is_head,
                                    HttpRequest& request,
                                    tcp::socket& socket);

/// Struct that contains parameters to check against an HTTP response.
/// When a field is std::nullopt, that particular property is not checked.
struct ResponseParams {
    /// Expected body content; if std::nullopt, the body is not checked.
    opt<std::string> body = std::nullopt;
    /// True if the request is a HEAD request. Note that head-check depends on whether body is provided.
    bool head = false;
    /// Expected content type of the response.
    std::string type = "application/json";
    /// Expected content length. If body is provided, len can be set to body.size(), otherwise this value is used.
    int len = 0;
    /// Expected HTTP status code; if std::nullopt, the status is not checked.
    opt<http::status> res = std::nullopt;
    /// List of allowed HTTP methods if the response indicates "Method Not Allowed".
    opt<vecstr> allowed = std::nullopt;
};

/// Validates that the provided StringResponse meets the expectations defined in ResponseParams.
/// Uses Catch2 assertions to verify response properties.
/// @param response The response to validate.
/// @param params The expected parameters for the response.
void CheckStringResponse(const StringResponse& response, const ResponseParams& params);
