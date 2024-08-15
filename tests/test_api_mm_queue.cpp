#include "socket_functions.hpp"
#include "api_functions.hpp"
#include "user.hpp"

TEST_CASE("ApiMMQueue", "[api][debug][matchmaking_queue]"){
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    
    std::string UNAUTHORIZED = serializer::SerializeError("invalid_admin", "the administrator password is missing or incorrect");
    std::string ENQUEUE_ERROR = serializer::SerializeError("enqueue_error", "error happened while enqueuing player (already in queue)");

    using Uuids = std::vector<um::Uuid>;
    SECTION("valid_response"){
    	Uuids queue = MMQueueSuccess(socket);
    	// dont check emptyness. other test metods call login functions so queue can be filled
		
        hh::RegistrationData rd;
        LoginData ld;
		int prev_queue_size = queue.size();
		
		for(int i = 0; i < 10; ++i){		
			rd = RegisterSuccess(socket);
			ld = LoginSuccess(socket, rd.login);
			bool res = EnqueueSuccess(socket, ld.token);
			REQUIRE(res == true);
			queue = MMQueueSuccess(socket);
			if (prev_queue_size == 1){
				REQUIRE(queue.empty());
			}
			else{
				REQUIRE(queue.size() == 1);
				REQUIRE(queue.back() == UserSuccess(socket, rd.login, rd.password).uuid);
			}
			prev_queue_size = queue.size();
		}
		
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
	    	StringResponse response = PlayerTokens(socket, "wrong login", "wrong password");
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is json, but wrong password"){
	    	StringResponse response = PlayerTokens(socket, "leedoor", "wrong password");
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is json, but wrong login"){
	    	StringResponse response = PlayerTokens(socket, "wrong login", "123qwe123");
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