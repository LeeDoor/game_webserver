#include "static_handler.hpp"
#include "response_builder.hpp"
#include "spdlog/spdlog.h"

namespace http_handler {
    StaticHandler::StaticHandler(HandlerParameters handler_parameters){
        serializer_ = handler_parameters.serializer;
        static_path_ = fs::weakly_canonical(handler_parameters.static_path);
    }

    void StaticHandler::Handle(HttpRequest&& request, SessionFunctions&& session_functions) {
        std::string path_str 
            = static_cast<std::string>(request.target().substr(0, request.target().find('?')));
        fs::path path = static_path_;
        path.concat(path_str);
        SessionData rns{request,session_functions};
        if(!IsSubdirectory(std::move(path))) {
            spdlog::info("not a subdirectory");
            return SendNoAccessError(rns);
        }
        if(!fs::exists(path) || fs::is_directory(path)) {
            return SendWrongPathError(rns);
        }
        return SendFile(std::move(path), rns);
    }

    void StaticHandler::SendFile(fs::path&& path, SessionData rns){
        ResponseBuilder<http::file_body> builder;
        rns.session_functions.send_file(std::move(builder.File(path, rns.request.method()).Status(status::ok).GetProduct()));
    }
    void StaticHandler::SendWrongPathError(SessionData rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("wrong_path", "file does not exists");
        rns.session_functions.send_string(std::move(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct()));
    }
    void StaticHandler::SendNoAccessError(SessionData rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("no_acess", "path is out of root");
        rns.session_functions.send_string(std::move(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct()));
    }


    bool StaticHandler::IsSubdirectory(fs::path&& path) {
        path = fs::weakly_canonical(path);
        for (auto b = static_path_.begin(), p = path.begin(); b != static_path_.end(); ++b, ++p) {
            if (p == path.end() || *p != *b) {
                return false;
            }
        }
        return true;
    }
}// http_handler