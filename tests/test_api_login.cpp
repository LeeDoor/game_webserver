#include "socket_functions.hpp"
using namespace serializer;
#include <catch2/catch_get_random_seed.hpp>

TEST_CASE("server logins players and returns valid token", "[api][login]"){
    net::io_context ioc;
    tcp::socket socket{ioc};

    ConnectSocket(ioc, socket);
    REQUIRE(socket.is_open());
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();

    SECTION("dd")

}