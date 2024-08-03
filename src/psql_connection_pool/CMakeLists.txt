#initializer's dependencies
set(LIBRARY_NAME psql_connection_pool)
#creating library
add_library(${LIBRARY_NAME}
    postgres_connection_pool.hpp
    postgres_connection_pool.cpp
)
