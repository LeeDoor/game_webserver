cmake_minimum_required(VERSION 3.28)
project(application VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
enable_testing()

find_package(Catch2 CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(redis++ CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(boost_algorithm REQUIRED CONFIG)
find_package(boost_asio REQUIRED CONFIG)
find_package(boost_beast REQUIRED CONFIG)
find_package(boost_program_options REQUIRED CONFIG)


#options
option(BUILD_TESTS "build with tests" ON)
set (DEPENDENCIES 
    initializer 
    server 
    http_handler 
    user_manager 
    token_manager 
    queue_manager 
    game_manager 
    serializer
    notification_system
    session_manager
    psql_connection_pool
)
foreach (dir IN ITEMS ${DEPENDENCIES})
    list(APPEND DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/${dir})
endforeach ()

add_subdirectory(src)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
