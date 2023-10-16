//
// Created by leedoor on 22.08.23.
//

#include "static_handler.hpp"
#include "response_maker.hpp"
#include <iostream>
namespace http_handler {
    StaticHandler::StaticHandler(std::shared_ptr<ISerializer> serializer){
        serializer_ = serializer;
    }

    void StaticHandler::HandleFile(HttpRequest&& request,
                            fs::path&& root, 
                            StrResponseSender&& string_sender,
                            FileResponseSender&& file_sender) {
        fs::path path = static_cast<std::string>(request.target());
        if(!IsSubdirectory(std::move(path), std::move(root))) {
            return SendNoAccessError(std::move(request), std::move(string_sender));
        }
        if(!fs::exists(path) || fs::is_directory(path)) {
            return SendWrongPathError(std::move(request), std::move(string_sender));
        }
        return SendFile(std::move(request), std::move(file_sender));
    }

    void StaticHandler::SendFile(HttpRequest&& request, FileResponseSender&& file_sender){
        file_sender(std::move(MakeFileResponse(std::move(request))));
    }
    void StaticHandler::SendWrongPathError(HttpRequest&& request, StrResponseSender&& string_sender){
        std::string body = serializer_->SerializeError("wrong_path", "file does not exists");
        return string_sender(std::move(MakeStringResponse(std::move(request), std::move(body))));
    }
    void StaticHandler::SendNoAccessError(HttpRequest&& request, StrResponseSender&& string_sender){
        std::string body = serializer_->SerializeError("no_acess", "path is out of root");
        return string_sender(std::move(MakeStringResponse(std::move(request), std::move(body))));
    }


    bool StaticHandler::IsSubdirectory(fs::path&& path, fs::path&& base) {
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