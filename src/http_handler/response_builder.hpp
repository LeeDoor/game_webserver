#pragma once
#include "http_types.hpp"

namespace http_handler{
    template<typename BodyType>
    class ResponseBuilder{
    public:
        void Reset(){
            response_ = {};
        }

        ResponseBuilder& BodyText(std::string&& body) {
            response_.body() = std::move(body);
            response_.set(http::field::content_type, TEXT_JSON); 
            return *this;
        }
        ResponseBuilder& File(const fs::path& path) {
            http::file_body::value_type file;
            beast::error_code ec;
            file.open(path.c_str(), beast::file_mode::read, ec);
            response_.body() = std::move(file);
            std::string extension = extensions[path.string().substr(1 + path.string().rfind("."))];
            response_.set(http::field::content_type, extension);
            return *this;
        }
        ResponseBuilder& Status(http::status status) {
            response_.result(status);
            return *this;
        }
        ResponseBuilder& Header(http::field field_name, std::string&& value){
            response_.set(field_name, std::move(value));
            return *this;
        }
        ResponseBuilder& Allow(const std::vector<http::verb>& methods){
            std::stringstream ss;
            for(int i = 0; i < methods.size() - 1; ++i)
                ss << methods[i] << ", ";
            ss << methods.back();
            response_.set(http::field::allow, ss.str());
            return *this;
        }

        http::response<BodyType> GetProduct(const HttpRequest& request){
            response_.content_length(response_.body().size());
            return std::move(response_);
        }
    private:
        http::response<BodyType> response_;

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
    };
}