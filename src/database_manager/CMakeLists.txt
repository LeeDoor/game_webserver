#initializer's dependencies
set(DEPENDENCIES  http_handler)
set(LIBRARY_NAME database_manager)
#creating library
add_library(${LIBRARY_NAME}
    i_user_data_manager.hpp
    user_data_postgres.cpp
    user_data_postgres.hpp
    user_data.hpp
    postgres_connection_pool.hpp
    postgres_connection_pool.cpp
    postgres_data_conversion.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME}  ${DEPENDENCIES} ${CONAN_LIBS})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()