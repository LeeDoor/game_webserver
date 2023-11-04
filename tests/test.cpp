#include <catch2/catch_test_macros.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

SCENARIO("server launches and responses to test api", "[launch][test_api]") {
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    using tcp = net::ip::tcp;
    net::io_context ioc;
 
    tcp::resolver resolver(ioc); 
    auto const results = resolver.resolve("127.0.0.1", "9999");

    tcp::socket socket(ioc);
    net::connect(socket, results.begin(), results.end());

    http::request<http::string_body> req{http::verb::get, "/api/test", 11};
    http::write(socket, req);

    beast::flat_buffer buffer;

    http::response<http::string_body> res;
    http::read(socket, buffer, res);
    INFO("body: " << res.body());
    
    CHECK(res.body() == "{\"LOL\":\"KEK\",\"SASI\":\"LALKA\"}");
    
    beast::error_code ec;
    socket.shutdown(tcp::socket::shutdown_both, ec);
    INFO("ec: " << ec.what());
    REQUIRE(ec.operator bool() == false);
}
