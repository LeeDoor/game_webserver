#add subdirectories
foreach (dir IN ITEMS ${DEPENDENCIES})
    add_subdirectory(${dir})
endforeach ()

#create exe file
add_executable(${PROJECT_NAME} main.cpp)

#///////////////////////
find_path(HIREDIS_HEADER hiredis)
target_include_directories(${PROJECT_NAME} PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
message("ABOBAAAA ${HIREDIS_LIB}")

# <------------ add redis-plus-plus dependency -------------->
# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(${PROJECT_NAME} PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
#//////////////////////////

#include folders for headers
target_include_directories(${PROJECT_NAME} PRIVATE ${DIRECTORIES})

#link conan libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${DEPENDENCIES} 
    ${HIREDIS_LIB}
    ${REDIS_PLUS_PLUS_LIB}
    ${Boost_LIBRARIES} 
    nlohmann_json::nlohmann_json
    spdlog::spdlog
) 

target_compile_options(${PROJECT_NAME} PRIVATE -frtti -Wall -Wextra -Wpedantic -Werror)