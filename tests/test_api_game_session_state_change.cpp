#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "json_serializer.hpp"
#include "session.hpp"
using namespace serializer;

TEST_CASE("ApiSessionState", "[api][game][session_state]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string URL_PARAMETERS_ERROR = serializer->SerializeError("url_parameters_error", "this api function requires url parameters");
    std::string WRONG_SESSIONID = serializer->SerializeError("wrong_sessionId", "no session with such sessionId");
    std::string UNAUTHORIZED = serializer->SerializeError("unathorized", "request must be authorized");
