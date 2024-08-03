#initializer's dependencies
set(DEPENDENCIES user_manager)
set(LIBRARY_NAME queue_manager)
#creating library
add_library(${LIBRARY_NAME}
    i_queue_manager.hpp
    queue_manager_redis.hpp
    queue_manager_redis.cpp
)

#including and linking dependencies
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()