#pragma once
#include "http_types.hpp"

namespace http_handler{
template<typename BodyType>
    class ResponseBuilder{
    public:
        //ONLY IF BodyType IS http::string_body
        //sets body string to given
        ResponseBuilder& BodyText(std::string&& body, http::verb method, bool can_head = true) {
            response_.content_length(body.size());
            if(method != http::verb::head || !can_head)
                response_.body() = std::move(body);
            response_.set(http::field::content_type, TEXT_JSON); 
            return *this;
        }
        //ONLY IF BodyType IS http::file_body
        //sets body file to given in path
        ResponseBuilder& File(const fs::path& path, http::verb method, bool can_head = true) {
            http::file_body::value_type file;
            beast::error_code ec;
            file.open(path.c_str(), beast::file_mode::read, ec);
            if(method != http::verb::head || !can_head)
                response_.body() = std::move(file);
            std::string extension = extensions[path.string().substr(1 + path.string().rfind("."))];
            response_.set(http::field::content_type, extension);
            response_.content_length(file.size());
            return *this;
        }
        //sets response status to given
        ResponseBuilder& Status(http::status status) {
            response_.result(status);
            return *this;
        }
        //sets any required header to given value
        ResponseBuilder& Header(http::field field_name, std::string&& value){
            response_.set(field_name, std::move(value));
            return *this;
        }
        //sets allow method to given methods
        ResponseBuilder& Allow(const std::vector<http::verb>& methods){
            std::stringstream ss;
            for(int i = 0; i < methods.size() - 1; ++i)
                ss << methods[i] << ", ";
            ss << methods.back();
            response_.set(http::field::allow, ss.str());
            return *this;
        }
        //sets content_length and returns response object
        http::response<BodyType> GetProduct(){
            response_.set(http::field::access_control_allow_origin, "*");
            response_.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
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