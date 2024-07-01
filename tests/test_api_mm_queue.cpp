#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "json_serializer.hpp"
#include "user_data.hpp"
using namespace serializer;

TEST_CASE("ApiMMQueue", "[api][debug][matchmaking_queue]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string UNAUTHORIZED = serializer->SerializeError("invalid_admin", "the administrator password is missing or incorrect");
    std::string ENQUEUE_ERROR = serializer->SerializeError("enqueue_error", "error happened while enqueuing player (already in queue)");

    using Uuids = std::vector<dm::Uuid>;
    SECTION("valid_response"){
    	Uuids given = MMQueueSuccess(socket, serializer);
    	// dont check emptyness. other test metods call login functions so queue can be filled
    }

    SECTION("admin_verification"){
    	SECTION ("server returns error when body is mess"){
	    	http::request<http::string_body> req{http::verb::get, MM_QUEUE_API, 11};

		    req.body() = "wrong admin credentials";
		    req.prepare_payload();

		    StringResponse response = GetResponseToRequest(false, req, socket);
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is json, but wrong headers"){
	    	http::request<http::string_body> req{http::verb::get, MM_QUEUE_API, 11};

		    req.body() = "{\"header1\":\"value1\",\"header2\",\"value2\"}";
		    req.prepare_payload();

		    StringResponse response = GetResponseToRequest(false, req, socket);
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is json, but wrong credentials"){
	    	StringResponse response = PlayerTokens(socket, serializer, "wrong login", "wrong password");
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is json, but wrong password"){
	    	StringResponse response = PlayerTokens(socket, serializer, "leedoor", "wrong password");
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is json, but wrong login"){
	    	StringResponse response = PlayerTokens(socket, serializer, "wrong login", "123qwe123");
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is empty json"){
	    	http::request<http::string_body> req{http::verb::get, MM_QUEUE_API, 11};;

		    req.body() = "{}";
		    req.prepare_payload();

		    StringResponse response = GetResponseToRequest(false, req, socket);
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
    }
}