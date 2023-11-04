cmake_minimum_required(VERSION 3.1)
project(executable VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)

#including conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#options
option(BUILD_TESTS "build with tests" OFF)

add_subdirectory(src)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()