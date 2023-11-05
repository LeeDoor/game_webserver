#pragma once
#include "api_function_builder.hpp"
#include "i_serializer.hpp"
#include "response_builder.hpp"
#include "players.hpp"
#include <map>

namespace http_handler {
    using namespace serializer;
    namespace ud = user_data;
    class ApiHandler :  public std::enable_shared_from_this<ApiHandler> {
    public:
        ApiHandler(std::shared_ptr<ISerializer> serializer, std::shared_ptr<ud::Players> players);
        void Init();
        
        void HandleApiFunction(HttpRequest&& request, ResponseSender&& sender);

    private:
        void BuildTargetsMap();

        //api functions
        void ApiGetTestJson(RequestNSender rns);
        void ApiRegister(RequestNSender rns);
        
        //api requests
        void SendSuccess(RequestNSender rns);
        
        void SendWrongApiFunction(RequestNSender rns);
        void SendWrongBodyData(RequestNSender rns);
        void SendLoginTaken(RequestNSender rns);

        void HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, RequestNSender rns);
        void SendWrongMethod(const ApiFunctionExecutor& executor, RequestNSender rns);
        void SendUndefinedError(const ApiFunctionExecutor& executor, RequestNSender rns);

        std::map<std::string, ApiFunctionExecutor> request_to_executor_;
        std::shared_ptr<ISerializer> serializer_;
        std::shared_ptr<ud::Players> players_;
    };

} // http_handler

