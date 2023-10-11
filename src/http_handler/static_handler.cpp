//
// Created by leedoor on 22.08.23.
//

#include "static_handler.hpp"
#include <iostream>
namespace http_handler {
    StaticHandler::StaticHandler(std::shared_ptr<ISerializer> serializer){
        serializer_ = serializer;
    }

    void StaticHandler::HandleFile(fs::path&& path,
                            fs::path&& root, 
                            StrResponseSender&& string_sender,
                            FileResponseSender&& file_sender) {
        if(!IsSubdirectory(path, root)) {
            return SendNoAccessError(std::move(string_sender));
        }
        if(!fs::exists(path)) {
            return SendWrongPathError(std::move(string_sender));
        }
        return SendFile(std::move(path), std::move(file_sender));
    }

    void StaticHandler::SendFile(const fs::path& path, FileResponseSender&& file_sender){
        FileResponse response;

        http::file_body::value_type file;
        beast::error_code ec;
        file.open(path.c_str(), beast::file_mode::read, ec);
        response.body() = std::move(file);
        
        response.prepare_payload();
        file_sender(std::move(response));
    }
    void StaticHandler::SendWrongPathError(StrResponseSender&& string_sender){
        StringResponse response;
        response.body() = serializer_->SerializeError("wrong_path", "path is out of root"); 
        response.content_length(response.body().size());
        return string_sender(std::move(response));
    }
    void StaticHandler::SendNoAccessError(StrResponseSender&& string_sender){

        std::cout << "no access\n";
    }


    bool StaticHandler::IsSubdirectory(fs::path& path, fs::path& base) {
        path = fs::weakly_canonical(path);
        base = fs::weakly_canonical(base);
        for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
            if (p == path.end() || *p != *b) {
                return false;
            }
        }
        return true;
    }
} // http_handler