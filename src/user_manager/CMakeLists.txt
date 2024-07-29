#initializer's dependencies
set(DEPENDENCIES http_handler connection_pool)
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
target_link_libraries(${LIBRARY_NAME}  ${DEPENDENCIES} ${CONAN_LIBS})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()