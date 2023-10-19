#pragma once
#include "http_types.hpp"

namespace http_handler{

    FileResponse MakeFileResponse(fs::path&& path, const HttpRequest& request);
    StringResponse MakeStringResponse(const HttpRequest& request, std::string&& body);

}