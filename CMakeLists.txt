cmake_minimum_required(VERSION 3.1)
project(hex_chess VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
#including conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src)