#initializer's dependencies
set(LIBRARY_NAME serializer)
#creating library
add_library(${LIBRARY_NAME}
    json_type_converter.hpp
    serializer.cpp
    serializer_basic.hpp
    serializer_game.hpp
    serializer_user.hpp
    serializer_http.hpp
    serializer_session.hpp
    type_serializer.cpp
    type_serializer.hpp
)

target_link_libraries(${LIBRARY_NAME} PRIVATE vt_game_structs)

target_include_directories(${LIBRARY_NAME} PUBLIC ${DIRECTORIES})