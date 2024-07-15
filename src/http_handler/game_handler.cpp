#include "game_handler.hpp"
#include "api_function_director.hpp"
#include "spdlog/spdlog.h"

#define BIND(func) (ExecutorFunction)std::bind( func, this->shared_from_this(), std::placeholders::_1) 

namespace http_handler{
    GameHandler::GameHandler(HandlerParameters handler_parameters)
    :responser_(handler_parameters.serializer),
     serializer_(handler_parameters.serializer),
     iqm_(handler_parameters.queue_manager),
     gm_(handler_parameters.game_manager),
     tm_(handler_parameters.token_manager)
      {}

    void GameHandler::Init() {
        ApiFunctionsParse();
    }
    void GameHandler::ApiFunctionsParse() {
        ApiFunctionDirector afd(serializer_, tm_);
        request_to_executor_ = {
            {"/api/game/enqueue", afd.GetRegister(BIND(&GameHandler::ApiEnqueue))},
        };
    }
    void GameHandler::Handle(HttpRequest&& request, SessionFunctions&& session_functions){
        std::string function = static_cast<std::string>(request.target());
        // removing url parameters
        std::string::size_type pos = function.find('?');
        function = function.substr(0, pos);

        SessionData rns {request, session_functions};
        if(request.method() == http::verb::options){
            ResponseBuilder<http::string_body> builder;
            http::response<http::string_body> product = 
                builder.Allow({http::verb::get, http::verb::head, http::verb::post})
                .Status(status::ok)
                .GetProduct();
            product.set(http::field::access_control_allow_origin, "*");
            product.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
            rns.session_functions.send_string(std::move(product));
            return;
        }
        if(request_to_executor_.contains(function)) {
            ApiFunctionExecutor& executor = request_to_executor_.at(function);
            ApiStatus result = executor.Execute(rns);
            if(result != ApiStatus::Ok)
                return responser_.HandleApiError(result, executor, rns);
        }
        else{
            responser_.SendWrongApiFunction(rns);
        }
    }
    void GameHandler::ApiEnqueue(SessionData rns){
        /*auto token = SenderAuthentication(rns.request);
        auto uuid = tm_->GetUuidByToken(token);
        bool res = iqm_->EnqueuePlayer(*uuid);
        if (!res)
            return responser_.SendCantEnqueue(rns);
        return responser_.SendSuccess(rns);*/
    }
}