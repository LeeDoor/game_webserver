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
        void SendSuccess(const StrResponseSender& sender);
        
        void SendWrongApiFunction(const StrResponseSender& sender);
        void SendWrongBodyData(const StrResponseSender& sender);
        void SendLoginTaken(const StrResponseSender& sender);

        void HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, const StrResponseSender& sender);
        void SendWrongMethod(const ApiFunctionExecutor& executor, const StrResponseSender& sender);
        void SendUndefinedError(const ApiFunctionExecutor& executor, const StrResponseSender& sender);

        std::map<std::string, ApiFunctionExecutor> request_to_executor_;
        std::shared_ptr<ISerializer> serializer_;
        std::shared_ptr<ud::Players> players_;
    };

} // http_handler

