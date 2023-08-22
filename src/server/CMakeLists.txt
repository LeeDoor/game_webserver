#initializer's dependencies
set(DEPENDENCIES http_handler)
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
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()