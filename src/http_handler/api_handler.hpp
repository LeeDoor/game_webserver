#pragma once
#include "api_function_builder.hpp"
#include "i_serializer.hpp"
#include "response_builder.hpp"
#include <map>

namespace http_handler {
    using namespace serializer;
    class ApiHandler :  public std::enable_shared_from_this<ApiHandler> {
    public:
        ApiHandler(std::shared_ptr<ISerializer> serializer);
        void Init();
        
        void HandleApiFunction(HttpRequest&& request, ResponseSender&& sender);

    private:
        void BuildTargetsMap();

        //api functions
        void ApiGetTestJson(const HttpRequest& request, const ResponseSender& sender);
        
        //api errors
        void HandleApiError(ApiStatus status, const ApiFunctionExecutor& executor, 
                        const HttpRequest& request, const StrResponseSender& sender);
        void SendWrongApiFunction(const HttpRequest& request, const StrResponseSender& sender);
        void SendWrongMethod(const ApiFunctionExecutor& executor, const HttpRequest& request, const StrResponseSender& sender);
        void SendUndefinedError(const ApiFunctionExecutor& executor, const HttpRequest& request, const StrResponseSender& sender);

        std::map<std::string, ApiFunctionExecutor> request_to_executor_;
        std::shared_ptr<ISerializer> serializer_;
    };

} // http_handler

