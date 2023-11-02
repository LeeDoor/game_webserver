cmake_minimum_required(VERSION 3.1)
project(executable VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
#including conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(src)
add_subdirectory(tests)