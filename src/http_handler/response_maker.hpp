#pragma once
#include "http_types.hpp"

namespace http_handler{

    FileResponse MakeFileResponse(http::status status, fs::path&& path, const HttpRequest& request);
    StringResponse MakeStringResponse(http::status status, std::string&& body, const HttpRequest& request);

}