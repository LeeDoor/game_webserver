#initializer's dependencies
set(DEPENDENCIES )
set(LIBRARY_NAME serializer)
#creating library
add_library(${LIBRARY_NAME}
    boost_json_includer.cpp
    i_serializer.hpp
    json_serializer.cpp
    json_serializer.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()