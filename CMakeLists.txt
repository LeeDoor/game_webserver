cmake_minimum_required(VERSION 3.24)
project(application VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(NLOHMANNJSON_ROOT "${PROJECT_SOURCE_DIR}/lib/json")
enable_testing()
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
find_package(libpqxx REQUIRED)
find_package(Boost 1.78 COMPONENTS program_options REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog CONFIG REQUIRED)

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
