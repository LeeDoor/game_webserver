#add sources
add_subdirectory(initializer)
add_subdirectory(server)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE server initializer)

#link conan libraries
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS} server initializer)