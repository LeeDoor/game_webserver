#pragma once
#include "http_types.hpp"

namespace http_handler{

    FileResponse MakeFileResponse(fs::path&& path, HttpRequest&& request);
    StringResponse MakeStringResponse(HttpRequest&& request, std::string&& body);

}