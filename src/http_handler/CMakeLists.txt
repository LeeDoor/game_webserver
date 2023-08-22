#initializer's dependencies
set(DEPENDENCIES )
set(LIBRARY_NAME http_handler)
#creating library
add_library(${LIBRARY_NAME}
        http_handler.hpp
        http_handler.cpp
)

set_target_properties(${LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)
#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
target_include_directories(${LIBRARY_NAME} PUBLIC "../${DEPENDENCIES}")
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()