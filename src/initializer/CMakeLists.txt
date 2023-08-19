add_library(initializer
    initializer.hpp
    initializer.cpp
)
target_link_libraries(initializer PRIVATE server)
target_include_directories(initializer PRIVATE ../server)