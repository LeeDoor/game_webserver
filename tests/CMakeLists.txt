add_executable(tests test_api_test.cpp test_jsonserializer.cpp)
target_include_directories(tests PRIVATE ../src/serializer)
target_link_libraries(tests ${CONAN_LIBS_CATCH2} ${CONAN_LIBS_BOOST} serializer)