#initializer's dependencies
set(DEPENDENCIES )
set(LIBRARY_NAME game_manager)
#creating library
add_library(${LIBRARY_NAME}
    game_manager.cpp
    game_manager.hpp
    session.hpp
    session.cpp
    session_id_generator.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()