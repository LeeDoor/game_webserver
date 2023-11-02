set(TESTS_NAME tests)
enable_testing()

add_executable(${TESTS_NAME} test.cpp)

target_include_directories(${TESTS_NAME} PRIVATE ../src/initializer)
message(${CONAN_LIBS})
target_link_libraries(${TESTS_NAME} ${CONAN_LIBS_CATCH2} ${CONAN_LIBS_BOOST} initializer)
add_test(
  NAME ${TESTS_NAME}
  COMMAND $<TARGET_FILE:${TESTS_NAME}>
  )
