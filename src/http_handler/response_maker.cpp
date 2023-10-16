#include "response_maker.hpp"
#include <map>
#include <iostream>

namespace http_handler{

    const std::string TEXT_HTML = "text/html";
    const std::string TEXT_PLAIN = "text/plain";
    const std::string TEXT_JSON = "application/json";
    const std::string PIC_JPG = "image/jpeg";
    const std::string PIC_TIF = "image/tiff";
    const std::string PIC_SVG = "image/svg+xml";
    const std::string UNKNOWN_TYPE = TEXT_PLAIN;



    std::unordered_map<std::string, std::string> extensions = {
        {"htm", TEXT_HTML },
        {"html", TEXT_HTML },
        {"css", "text/css" },
        {"txt", "text/plain" },
        {"js", "text/javascript" },
        {"json", "application/json" },
        {"xml", "application/xml" },
        {"png", "image/png" },
        {"jpg", PIC_JPG },
        {"jpe", PIC_JPG },
        {"jpeg", PIC_JPG },
        {"gif", "image/gif" },
        {"bmp", "image/bmp" },
        {"ico", "image/vnd.microsoft.icon" },
        {"tiff", PIC_TIF },
        {"tif", PIC_TIF },
        {"svg", PIC_SVG },
        {"svgz", PIC_SVG },
        {"mp3", "audio/mpeg" },

    };
    FileResponse MakeFileResponse(HttpRequest&& request) {
        FileResponse response;

        response.result(http::status::ok);

        auto path = request.target();
        std::string extension = extensions[static_cast<std::string>(path.substr(1 + path.rfind(".")))];
        response.set(http::field::content_type, extension);

        http::file_body::value_type file;
        beast::error_code ec;
        file.open(path.data(), beast::file_mode::read, ec);
        response.body() = std::move(file);

        response.prepare_payload();
        return response;
    }
    StringResponse MakeStringResponse(HttpRequest&& request, std::string&& body) {
        StringResponse response;

        response.result(http::status::ok);

        response.set(http::field::content_type, TEXT_JSON);
        response.body() = std::move(body); 

        response.content_length(response.body().size());
        return response;
    }
}