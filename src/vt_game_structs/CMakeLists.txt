#initializer's dependencies
set(LIBRARY_NAME vt_game_structs)
#creating library
add_library(${LIBRARY_NAME}
    object.cpp
    object_placed.cpp
    bomb.cpp

    object_directed.cpp
    gun.cpp
    bullet.cpp 
)
target_include_directories(${LIBRARY_NAME} PUBLIC ${DIRECTORIES})