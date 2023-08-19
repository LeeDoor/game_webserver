cmake_minimum_required(VERSION 3.1)
project(hex_chess VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)

#including conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#add cpp files
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/initializer/initializer.cpp
        src/initializer/initializer.hpp
        src/server/http_server.cpp
        src/server/http_server.hpp
)
target_include_directories(${PROJECT_NAME} PRIVATE src/server)
#link conan libraries
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
