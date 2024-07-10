#initializer's dependencies
set(DEPENDENCIES game_manager)
set(LIBRARY_NAME matchmaking_system)
#creating library
add_library(${LIBRARY_NAME}
    i_queue_manager.hpp
    queue_manager_redis.hpp
    queue_manager_redis.cpp
    matchmaking_ballancer.hpp
    matchmaking_ballancer.cpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()