#initializer's dependencies
set(LIBRARY_NAME queue_manager)
#creating library
add_library(${LIBRARY_NAME}
    i_queue_manager.hpp
    queue_manager_redis.hpp
    queue_manager_redis.cpp
)

target_include_directories(${LIBRARY_NAME} PUBLIC ${DIRECTORIES})