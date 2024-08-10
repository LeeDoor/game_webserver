#list with all libraries

#add subdirectories
foreach (dir IN ITEMS ${DEPENDENCIES})
    add_subdirectory(${dir})
endforeach ()

#create exe file
add_executable(${PROJECT_NAME} main.cpp)

#include folders for headers
target_include_directories(${PROJECT_NAME} PRIVATE ${DIRECTORIES})

#link conan libraries
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES} ${CONAN_LIBS}) 

target_compile_options(${PROJECT_NAME} PRIVATE -frtti)