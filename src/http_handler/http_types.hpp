#pragma once
#include <boost/beast.hpp>
#include <filesystem>
#include "user_data.hpp"
namespace fs = std::filesystem;
namespace net = boost::asio;
namespace sys = boost::system;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = net::ip::tcp;
using status = http::status;
typedef http::request <http::string_body> HttpRequest;
typedef http::response <http::string_body> StringResponse;
typedef http::response <http::file_body> FileResponse;
typedef std::function<void(StringResponse&&)> StrResponseSender;
typedef std::function<void(FileResponse&&)> FileResponseSender;

namespace http_handler{
    struct SessionFunctions{
        StrResponseSender send_string;
        FileResponseSender send_file;
    }; 
    struct SessionData{
        HttpRequest request;
        SessionFunctions session_functions;
    };
}