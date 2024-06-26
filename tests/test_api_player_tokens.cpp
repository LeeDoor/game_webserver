#include "socket_functions.hpp"
#include "api_functions.hpp"
using namespace serializer;
#include "token.hpp"
#include "json_serializer.hpp"

TEST_CASE ("player_tokens", "[api][debug][player_tokens]"){
    net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string UNAUTHORIZED = serializer->SerializeError("invalid_admin", "the administrator password is missing or incorrect");
    
    using StringMap = std::map<token_manager::Token, dm::Uuid>;
    SECTION ("server returns valid map when authorized"){
    	StringMap sm_given = PlayerTokensSuccess(socket, serializer);
    	// dont check emptyness. other test metods call login functions so map can be filled
    	// all tests run with catch2's preferable order and probably async.
    }
    SECTION ("checking logged users in map"){
    	StringMap sm_given = PlayerTokensSuccess(socket, serializer);
        hh::RegistrationData rd = RegisterSuccess(socket, serializer);
        LoginData ld = LoginSuccess(socket, rd.login, serializer);
    	CHECK(!sm_given.contains(ld.token));
    	sm_given = PlayerTokensSuccess(socket, serializer);
    	CHECK(sm_given.contains(ld.token));
    	CHECK(!sm_given[ld.token].empty());
    }
    SECTION ("checking logged users in map"){
    	StringMap sm_given = PlayerTokensSuccess(socket, serializer);

        std::vector<hh::RegistrationData> rds;
        rds.reserve(10);
        for(int i = 0; i < 10; ++i){
        	rds.push_back(RegisterSuccess(socket, serializer));
        }
        std::vector<LoginData> lds;
        lds.reserve(10);
        for(int i = 0; i < 10; ++i){
        	lds.push_back(LoginSuccess(socket, rds[i].login, serializer));
        } 
        for(int i = 0; i < 10; ++i){
    		CHECK(!sm_given.contains(lds[i].token));
        } 
    	sm_given = PlayerTokensSuccess(socket, serializer);
        for(int i = 0; i < 10; ++i){
	    	CHECK(sm_given.contains(lds[i].token));
			CHECK(!sm_given[lds[i].token].empty());
        } 
    }
    SECTION ("server returns error when body is mess"){
    	http::request<http::string_body> req{http::verb::get, PLAYER_TOKENS_API, 11};

	    req.body() = "wrong admin credentials";
	    req.prepare_payload();

	    StringResponse response = GetResponseToRequest(false, req, socket);
	    CheckStringResponse(response, {
	    	.body=UNAUTHORIZED,
	    	.res=http::status::unauthorized,
	    });
	}
    SECTION ("server returns error when body is json, but wrong headers"){
    	http::request<http::string_body> req{http::verb::get, PLAYER_TOKENS_API, 11};

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
    	http::request<http::string_body> req{http::verb::get, PLAYER_TOKENS_API, 11};;

	    req.body() = "{}";
	    req.prepare_payload();

	    StringResponse response = GetResponseToRequest(false, req, socket);
	    CheckStringResponse(response, {
	    	.body=UNAUTHORIZED,
	    	.res=http::status::unauthorized,
	    });
	}
}