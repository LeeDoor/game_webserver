set(TESTS_NAME tests)
add_executable(${TESTS_NAME} test.cpp)

target_link_libraries(${TESTS_NAME} ${CONAN_LIBS_CATCH2} ${CONAN_LIBS_BOOST} )
  
add_test(
    NAME ${TESTS_NAME}
    COMMAND $<TARGET_FILE:${TESTS_NAME}>
)

