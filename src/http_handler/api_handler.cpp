#include "api_handler.hpp"
#include "get_token_from_header.hpp"
#include "send_manager.hpp"
#include "response_builder.hpp"
#include "api_function_executor.hpp"
#include "send_manager_http.hpp"

namespace http_handler{
    void ApiHandler::Handle(HttpRequest&& request, SessionFunctions&& session_functions){
        std::string function = static_cast<std::string>(request.target());
        // removing url parameters
        std::string::size_type pos = function.find('?');
        function = function.substr(0, pos);

        SessionData rns {std::move(request), std::move(session_functions)};
        if(request.method() == http::verb::options){
            ResponseBuilder<http::string_body> builder;
            http::response<http::string_body> product = 
                builder.Allow({http::verb::get, http::verb::head, http::verb::post, http::verb::options})
                .Status(status::ok)
                .GetProduct();
            product.set(http::field::access_control_allow_origin, "*");
            product.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
            rns.session_functions.send_string(std::move(product));
            return;
        }
        if(request_to_executor_.contains(function)) {
            ApiFunctionExecutor& executor = request_to_executor_.at(function);
            ApiStatus result = executor.Execute(std::move(rns));
            if(result != ApiStatus::Ok)
                return HandleApiError(result, executor, rns);
        }
        else{
            SendWrongApiFunction(rns);
        }
    }

    tokenm::Token ApiHandler::SenderAuthentication(const HttpRequest& request) {
        auto auth_iter = request.find(http::field::authorization);
        std::optional<tokenm::Token> token = GetTokenFromHeader(auth_iter->value().to_string());
        return *token;
    }

    std::map<std::string, std::string> ApiHandler::ParseUrlParameters(const HttpRequest& request){
        std::map<std::string, std::string> res;
        std::string function = request.target().to_string();
        std::string::size_type pos = function.find('?');
        if (pos == std::string::npos)
            return res;
        std::string params = function.substr(pos + 1);
        pos = params.find('&');
        std::string pair_s;
        do {
            pair_s = params.substr(0, pos);
            params.erase(0, pos + 1);
            res.insert(ParseUrlPair(std::move(pair_s)));
        } while ((pos = params.find('&')) != std::string::npos);
        res.insert(ParseUrlPair(std::move(params)));
        return res;
    }
    std::pair<std::string, std::string> ApiHandler::ParseUrlPair(std::string&& pair) {
        // if line does not have equality sign (e.g. "/api/function?parameter1&parameter2")
        // then these parameters are being parsed as 
        // {"parameter1":"parameter1", "parameter2":"parameter2"}
        return {pair.substr(0, pair.find('=')), pair.substr(pair.find('=') + 1)};
    }
}