#initializer's dependencies
set(LIBRARY_NAME vt_game_structs)
#creating library
add_library(${LIBRARY_NAME}
    object.cpp
    bomb.cpp
    gun.cpp
    bullet.cpp 
    event.cpp
    custom_events.cpp
    directed_object.cpp
    player.cpp
)
target_include_directories(${LIBRARY_NAME} PUBLIC ${DIRECTORIES})