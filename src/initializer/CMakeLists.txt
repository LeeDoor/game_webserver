#initializer's dependencies
set(LIBRARY_NAME initializer)
#creating library
add_library(${LIBRARY_NAME}
    initializer.hpp
    initializer.cpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} 
    PRIVATE server
    PRIVATE user_manager 
    PRIVATE token_manager 
    PRIVATE queue_manager 
    PRIVATE game_manager 
    PRIVATE session_manager
    PRIVATE notification_system
)
target_include_directories(${LIBRARY_NAME} 
    PRIVATE "../server"
    PRIVATE "../user_manager" 
    PRIVATE "../token_manager" 
    PRIVATE "../queue_manager" 
    PRIVATE "../game_manager" 
    PRIVATE "../session_manager"
    PRIVATE "../notification_system"
)
