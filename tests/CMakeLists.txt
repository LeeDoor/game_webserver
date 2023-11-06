include (Catch)

macro (create_test EXEC_NAME FILES_NAME INCLUDE_DIRS LIBS)
    add_executable(${EXEC_NAME} ${FILES_NAME})
    target_include_directories(${EXEC_NAME} PRIVATE ${INCLUDE_DIRS})
    target_link_libraries(${EXEC_NAME} ${LIBS})
    add_test(
        NAME ${EXEC_NAME}
        COMMAND $<TARGET_FILE:${EXEC_NAME}>
    )
endmacro()

create_test(test_api_test test_api_test.cpp ../src/serializer "${CONAN_LIBS_CATCH2};${CONAN_LIBS_BOOST};serializer")
create_test(test_serialization test_serialization.cpp ../src/serializer "${CONAN_LIBS_CATCH2};serializer")
