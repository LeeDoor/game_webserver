#pragma once
#include "api_function_builder.hpp"
#include "i_serializer.hpp"
#include "response_maker.hpp"
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
        void ApiGetTestJson(HttpRequest&& request, ResponseSender&& sender);
        
        //api errors
        void SendWrongApiFunction(HttpRequest&& request, ResponseSender&& sender);

        std::map<std::string, ApiFunctionExecutor> request_to_executor_;
        std::shared_ptr<ISerializer> serializer_;
    };

} // http_handler

