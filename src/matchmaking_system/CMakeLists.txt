#initializer's dependencies
set(DEPENDENCIES )
set(LIBRARY_NAME matchmaking_system)
#creating library
add_library(${LIBRARY_NAME}
    mm_queue.cpp
    mm_queue.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()