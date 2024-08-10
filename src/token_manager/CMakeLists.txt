#initializer's dependencies
set(LIBRARY_NAME token_manager)
#creating library
add_library(${LIBRARY_NAME}
    i_token_manager.hpp
    token_manager_redis.hpp
    token_manager_redis.cpp
    token_generator.hpp
    token_generator.cpp
    token.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC user_manager)
target_include_directories(${LIBRARY_NAME} PUBLIC ${DIRECTORIES})