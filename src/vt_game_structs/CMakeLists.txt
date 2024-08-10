#initializer's dependencies
set(LIBRARY_NAME vt_game_structs)
#creating library
add_library(${LIBRARY_NAME}
    bomb.cpp
    object.cpp
)
target_include_directories(${LIBRARY_NAME} PUBLIC ${DIRECTORIES})