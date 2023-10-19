//
// Created by leedoor on 22.08.23.
//

#include "static_handler.hpp"
#include "response_builder.hpp"
#include <iostream>
namespace http_handler {
    StaticHandler::StaticHandler(std::shared_ptr<ISerializer> serializer){
        serializer_ = serializer;
    }

    void StaticHandler::HandleFile(HttpRequest&& request, fs::path&& root, ResponseSender&& sender) {
        std::string path_str = static_cast<std::string>(request.target());
        fs::path path = root.concat(path_str);
        if(!IsSubdirectory(std::move(path), std::move(root))) {
            std::cout << "should be called when user writes root like 0.0.0.0:port/../../../forbidden_folder/passwords.txt"
           " but boost beast not allowing sockets send this kind of targets\n";
            return SendNoAccessError(sender.string);
        }
        if(!fs::exists(path) || fs::is_directory(path)) {
            return SendWrongPathError(sender.string);
        }
        return SendFile(std::move(path), sender.file);
    }

    void StaticHandler::SendFile(fs::path&& path, const FileResponseSender& sender){
        ResponseBuilder<http::file_body> builder;
        sender(std::move(builder.File(path).Status(status::ok).GetProduct()));
    }
    void StaticHandler::SendWrongPathError(const StrResponseSender& sender){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("wrong_path", "file does not exists");
        sender(std::move(builder.BodyText(std::move(body)).Status(status::bad_request).GetProduct()));
    }
    void StaticHandler::SendNoAccessError(const StrResponseSender& sender){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("no_acess", "path is out of root");
        sender(std::move(builder.BodyText(std::move(body)).Status(status::bad_request).GetProduct()));
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