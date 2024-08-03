#initializer's dependencies
set(DEPENDENCIES user_manager http_handler token_manager game_manager session_manager)
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

#including and linking dependencies
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()