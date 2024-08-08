#initializer's dependencies
set(DEPENDENCIES user_manager game_manager http_handler)
set(LIBRARY_NAME notification_system)
#creating library
add_library(${LIBRARY_NAME}
    queue_notifier.hpp
    queue_notifier.cpp
    session_state_notifier.cpp
    session_state_notifier.hpp
    thread_lock.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} 
    PUBLIC game_manager
)
target_include_directories(${LIBRARY_NAME} 
    PUBLIC "../http_handler"
    PUBLIC "../game_manager"
)