#include "socket_functions.hpp"
#include "api_functions.hpp"
using namespace serializer;
#include "json_serializer.hpp"

TEST_CASE("ApiUserData", "[api][debug][user_data]") {
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string UNAUTHORIZED = serializer->SerializeError("invalid_admin", "the administrator password is missing or incorrect");
    std::string NO_SUCH_USER_UUID = serializer->SerializeError("user_not_found", "no user with provided uuid found");
    std::string NO_SUCH_USER_LP = serializer->SerializeError("user_not_found", "no user with provided login and password found");
    
    SECTION("server returns valid user data by uuid"){
    	hh::RegistrationData rd = RegisterSuccess(socket, serializer);
    	LoginData ld = LoginSuccess(socket, rd.login, serializer);
    	auto map = PlayerTokensSuccess(socket, serializer);
    	REQUIRE(map.contains(ld.token)); // player_tokens error
		dm::UserData ud = UserDataSuccess(socket, serializer, map[ld.token]);
		CHECK(ud.login == rd.login);    	
		CHECK(ud.password == rd.password);    	
    }

    SECTION("server returns valid user data by login and password"){
    	hh::RegistrationData rd = RegisterSuccess(socket, serializer);
    	LoginData ld = LoginSuccess(socket, rd.login, serializer);
    	auto map = PlayerTokensSuccess(socket, serializer);
    	REQUIRE(map.contains(ld.token)); // player_tokens error
		dm::UserData ud = UserDataSuccess(socket, serializer, rd.login, rd.password);
		CHECK(ud.uuid == map[ld.token]);    	
    }
}