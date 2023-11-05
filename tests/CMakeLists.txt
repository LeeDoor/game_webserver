set(TESTS_NAME tests)

add_executable(${TESTS_NAME} default_api_test.cpp)

target_include_directories(${TESTS_NAME} PRIVATE ../src/serializer)
target_link_libraries(${TESTS_NAME} ${CONAN_LIBS_CATCH2} ${CONAN_LIBS_BOOST} serializer )
  
add_test(
    NAME ${TESTS_NAME}
    COMMAND $<TARGET_FILE:${TESTS_NAME}>
)

