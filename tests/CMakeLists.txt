add_executable(test_application 
    api_functions.cpp
    api_functions.hpp
    socket_functions.hpp 
    socket_functions.cpp 

    test_jsonserializer.cpp
    test_api_register.cpp
    test_api_login.cpp
    test_api_profile.cpp
    test_api_player_tokens.cpp
)
target_include_directories(test_application PRIVATE 
    ../src/serializer 
    ../src/token_manager 
    ../src/http_handler
)
target_link_libraries(test_application ${CONAN_LIBS_CATCH2} ${CONAN_LIBS_BOOST} serializer)