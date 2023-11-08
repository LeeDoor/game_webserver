#initializer's dependencies
set(DEPENDENCIES serializer database_manager)
set(LIBRARY_NAME http_handler)
#creating library
add_library(${LIBRARY_NAME}
        http_types.hpp
        http_handler.hpp
        http_handler.cpp
        api_handler.cpp
        api_handler.hpp
        static_handler.cpp
        static_handler.hpp
        api_function_executor.cpp
        api_function_executor.hpp
        api_function_builder.cpp
        api_function_builder.hpp
        api_function.cpp
        api_function.hpp
        response_builder.hpp
        handler_parameters.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()