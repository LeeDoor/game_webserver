cmake_minimum_required(VERSION 3.1)
project(application VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
enable_testing()
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
    vt_game_structs
)
foreach (dir IN ITEMS ${DEPENDENCIES})
    list(APPEND DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/${dir})
endforeach ()

add_subdirectory(src)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()