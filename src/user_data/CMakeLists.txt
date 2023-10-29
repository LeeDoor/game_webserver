#initializer's dependencies
set(DEPENDENCIES )
set(LIBRARY_NAME user_data)
#creating library
add_library(${LIBRARY_NAME}
    player_data.hpp
    players.cpp
    players.hpp
    registration_data.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()