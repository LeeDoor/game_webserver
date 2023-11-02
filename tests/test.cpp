#include "initializer.hpp"
#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>

SCENARIO("server launches and responses to test api", "[launch][test_api]") {
    GIVEN("initializer"){
        initializer::Initializer in;
        WHEN ("server is launched") {
        }
    }
    
}