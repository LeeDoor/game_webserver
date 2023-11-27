#initializer's dependencies
set(DEPENDENCIES)
set(LIBRARY_NAME token_manager)
#creating library
add_library(${LIBRARY_NAME}
    token_to_uuid.cpp
    token_to_uuid.hpp
    token_generator.hpp
    token_generator.cpp
    token.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()