cmake_minimum_required(VERSION 3.1)
project(executable VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
enable_testing()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#including conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#options
option(BUILD_TESTS "build with tests" ON)
add_subdirectory(src)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()