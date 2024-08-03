#initializer's dependencies
set(LIBRARY_NAME server)
#creating library
add_library(${LIBRARY_NAME}
    http_server.hpp
    session.hpp
    listener.hpp
    http_server.cpp
    session.cpp
    listener.cpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} 
    PUBLIC http_handler
)
target_include_directories(${LIBRARY_NAME} 
    PUBLIC "../http_handler"
)