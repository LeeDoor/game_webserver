#initializer's dependencies
set(DEPENDENCIES server)

#creating library
add_library(initializer
    initializer.hpp
    initializer.cpp
)

#including and linking dependencies
target_link_libraries(initializer PRIVATE ${DEPENDENCIES})
target_include_directories(initializer PRIVATE "../${DEPENDENCIES}")