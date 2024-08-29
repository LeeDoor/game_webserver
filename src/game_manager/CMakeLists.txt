#initializer's dependencies
set(LIBRARY_NAME game_manager)
#creating library
add_library(${LIBRARY_NAME}
    game_manager.cpp
    game_manager.hpp
    session.hpp
    session.cpp
    session_id_generator.hpp
    matchmaking_balancer.hpp
    matchmaking_balancer.cpp
    state.hpp
    player.hpp
    bomb.hpp
    object.hpp
    event_manager.hpp
    event_manager.cpp
    gun.hpp
    bullet.hpp
    object.cpp
    bomb.cpp
    gun.cpp
    bullet.cpp 
    event.cpp
    custom_events.cpp
    directed_object.cpp
    player.cpp
    state.cpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} 
    PUBLIC session_manager
    PUBLIC http_handler

    PRIVATE notification_system
)
target_include_directories(${LIBRARY_NAME} PUBLIC ${DIRECTORIES})