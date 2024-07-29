#initializer's dependencies
set(DEPENDENCIES )
set(LIBRARY_NAME connection_pool)
#creating library
add_library(${LIBRARY_NAME}
    postgres_connection_pool.hpp
    postgres_connection_pool.cpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PRIVATE ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PRIVATE "../${dir}")
endforeach ()