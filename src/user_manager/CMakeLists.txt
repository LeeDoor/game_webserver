#initializer's dependencies
set(LIBRARY_NAME user_manager)
#creating library
add_library(${LIBRARY_NAME}
    i_user_manager.hpp
    user_manager_postgres.cpp
    user_manager_postgres.hpp
    user.hpp
    postgres_data_conversion.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC psql_connection_pool)
target_include_directories(${LIBRARY_NAME} PUBLIC ${DIRECTORIES})