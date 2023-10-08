//
// Created by leedoor on 22.08.23.
//

#include "static_handler.hpp"
#include <iostream>
namespace http_handler {
    void StaticHandler::HandleFile(fs::path&& path,
                            fs::path&& root, 
                            StrResponseSender&& stringSender,
                            FileResponseSender&& fileSender) {
        if(!IsSubdirectory(path, root)) {
            return SendNoAccessError(std::move(stringSender));
        }
        if(!fs::exists(path)) {
            return SendWrongPathError(std::move(stringSender));
        }
        std::cout << "file sending\n";
        return SendFile(std::move(path), std::move(fileSender));
    }

    void StaticHandler::SendFile(const fs::path& path, FileResponseSender&& sender){
        FileResponse response;

        http::file_body::value_type file;
        beast::error_code ec;
        file.open(path.c_str(), beast::file_mode::read, ec);
        response.body() = std::move(file);
        
        response.prepare_payload();
        sender(std::move(response));
    }
    void StaticHandler::SendWrongPathError(StrResponseSender&& sender){

        std::cout << "wrong path\n";
    }
    void StaticHandler::SendNoAccessError(StrResponseSender&& sender){

        std::cout << "no access\n";
    }


    bool StaticHandler::IsSubdirectory(fs::path& path, fs::path& base) {
        path = fs::weakly_canonical(path);
        base = fs::weakly_canonical(base);
        std::cout << "current path is: " << path << "\nbase path is: " << base << std::endl;

        for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
            if (p == path.end() || *p != *b) {
                std::cout << "RETURN FALSE with *p " << *p << " *b " << *b << std::endl; 
                return false;
            }
        }
        return true;
    }
} // http_handler