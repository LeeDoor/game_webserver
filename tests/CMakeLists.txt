add_executable(tests 
    api_functions.cpp
    api_functions.hpp
    socket_functions.hpp 
    socket_functions.cpp 

    test_jsonserializer.cpp
    test_api_register.cpp
    test_api_login.cpp
)
target_include_directories(tests PRIVATE ../src/serializer ../src/token_manager)
target_link_libraries(tests ${CONAN_LIBS_CATCH2} ${CONAN_LIBS_BOOST} serializer)