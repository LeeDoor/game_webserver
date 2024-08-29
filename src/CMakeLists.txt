#add subdirectories
foreach (dir IN ITEMS ${DEPENDENCIES})
    add_subdirectory(${dir})
endforeach ()

#create exe file
add_executable(${PROJECT_NAME} main.cpp)

#include folders for headers
target_include_directories(${PROJECT_NAME} PRIVATE ${DIRECTORIES})

#link conan libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${DEPENDENCIES} 
    Boost::algorithm 
    Boost::asio 
    Boost::beast 
    Boost::program_options 
    nlohmann_json::nlohmann_json 
    redis++::redis++_static 
    spdlog::spdlog
) 

target_compile_options(${PROJECT_NAME} PRIVATE -frtti -Wall -Wextra -Wpedantic -Werror)