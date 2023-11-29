#initializer's dependencies
set(DEPENDENCIES database_manager http_handler)
set(LIBRARY_NAME serializer)
#creating library
add_library(${LIBRARY_NAME}
    json_type_converter.hpp
    i_serializer.hpp
    json_serializer.cpp
    json_serializer.hpp
    type_serializer.cpp
    type_serializer.hpp
)

#including and linking dependencies
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()